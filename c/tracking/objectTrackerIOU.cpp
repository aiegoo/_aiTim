/*uconcreative

#include "objectTrackerIOU.h"


// constructor
objectTrackerIOU::objectTrackerIOU( uint32_t minFrames, uint32_t dropFrames, float overlapThreshold )
{
	mIDCount = 0;
	mFrameCount = 0;
	
	mMinFrames = minFrames;
	mDropFrames = dropFrames;
	
	mOverlapThreshold = overlapThreshold;
	
	mTracks.reserve(128);
}


// destructor
objectTrackerIOU::~objectTrackerIOU()
{

}


// Create
objectTrackerIOU* objectTrackerIOU::Create( uint32_t minFrames, uint32_t dropFrames, float overlapThreshold )
{
	objectTrackerIOU* tracker = new objectTrackerIOU(minFrames, dropFrames, overlapThreshold);
	
	if( !tracker )
		return NULL;

	return tracker;
}


// Create
objectTrackerIOU* objectTrackerIOU::Create( const commandLine& cmdLine )
{
	return Create(cmdLine.GetUnsignedInt("tracker-min-frames", OBJECT_TRACKER_DEFAULT_MIN_FRAMES),
			    cmdLine.GetUnsignedInt("tracker-drop-frames", OBJECT_TRACKER_DEFAULT_DROP_FRAMES),
			    cmdLine.GetFloat("tracker-overlap", OBJECT_TRACKER_DEFAULT_OVERLAP_THRESHOLD));
}


// Create
objectTrackerIOU* objectTrackerIOU::Create( int argc, char** argv )
{
	return Create(commandLine(argc, argv));
}


// find the detection with the highest IOU that exceeds the given threshold
int findBestIOU( const detectNet::Detection& track, detectNet::Detection* detections, int numDetections, float threshold=0.5f )
{
	int maxDetection = -1;
	float maxIOU = 0.0f;
	
	for( int n=0; n < numDetections; n++ )
	{
		if( detections[n].TrackID >= 0 )
			continue; // this bbox is already a match for another track
		
		if( detections[n].ClassID != track.ClassID )
			continue;
		
		const float IOU = track.IOU(detections[n]);
		
		if( IOU >= threshold && IOU > maxIOU )
		{
			maxIOU = IOU;
			maxDetection = n;
		}
	}
	
	return maxDetection;
}
		

// Process
int objectTrackerIOU::Process( void* input, uint32_t width, uint32_t height, imageFormat format, detectNet::Detection* detections, int numDetections )
{
	if( !mEnabled )
		return numDetections;
	
	// update active tracks
	for( int n=0; n < mTracks.size(); n++ )
	{
		const int bestMatch = findBestIOU(mTracks[n], detections, numDetections, mOverlapThreshold);
		
		if( bestMatch >= 0 )
		{
			detections[bestMatch].TrackID = (mTracks[n].TrackFrames == mMinFrames) ? mIDCount++ : mTracks[n].TrackID;
			detections[bestMatch].TrackFrames = mTracks[n].TrackFrames + 1;
			detections[bestMatch].TrackStatus = (detections[bestMatch].TrackFrames >= mMinFrames) ? 1 : 0;
			detections[bestMatch].TrackLost = 0;
			
			mTracks[n] = detections[bestMatch];
			
			LogVerbose(LOG_TRACKER "updated track %i -> class=%u status=%i frames=%i\n", detections[n].TrackID, detections[n].ClassID, detections[n].TrackStatus, detections[n].TrackFrames);
		}
		else
		{
			mTracks[n].TrackLost++;
			
			if( mTracks[n].TrackLost >= mDropFrames )
				mTracks[n].TrackStatus = -1;
		}
	}
	
	// add new tracks
	for( int n=0; n < numDetections; n++ )
	{
		if( detections[n].TrackID >= 0 )
			continue;
		
		detections[n].TrackID = -1;
		detections[n].TrackStatus = 0;
		detections[n].TrackFrames = 0;
		detections[n].TrackLost = 0;
		
		mTracks.push_back(detections[n]);
		
		LogVerbose(LOG_TRACKER "added track %i -> class=%u\n", detections[n].TrackID, detections[n].ClassID);
	}
	
	// add valid tracks to the output array
	numDetections = 0;
	
	for( int n=0; n < mTracks.size(); n++ )
	{
		if( mTracks[n].TrackFrames >= mMinFrames )
			detections[numDetections++] = mTracks[n];
	}
	
	// remove dropped tracks
	for( auto iter = mTracks.begin(); iter != mTracks.end(); )
	{
		if( iter->TrackStatus < 0 )
		{
			LogVerbose(LOG_TRACKER "dropped track %i -> class=%u frames=%i\n", iter->TrackID, iter->ClassID, iter->TrackFrames);
			iter = mTracks.erase(iter);
		}
		else
		{
			++iter;
		}
	}
	
	mFrameCount++;
	return numDetections;
}
