/*uconcreative

#include "detectNet.h" 
#include "objectTracker.h"


#include "objectTrackerIOU.h"
#include "objectTrackerKLT.h"


// constructor
objectTracker::objectTracker()
{
	mEnabled = true;
}


// destructor
objectTracker::~objectTracker()
{
	
}


// Create
objectTracker* objectTracker::Create( objectTracker::Type type )
{
	objectTracker* tracker = NULL;
	
	if( type == KLT )
	{
	#if HAS_VPI
		tracker = objectTrackerKLT::Create();
	#else
		LogError(LOG_TRACKER "couldn't create KLT tracker (not built with VPI enabled)\n");
	#endif
	}
	else if( type == IOU )
	{
		tracker = objectTrackerIOU::Create();
	}
	
	if( !tracker )
		return NULL;

	return tracker;
}


// Create
objectTracker* objectTracker::Create( const commandLine& cmdLine )
{
	Type type = IOU;
	
	const bool useDefault = cmdLine.GetFlag("tracking");
	const char* typeStr = cmdLine.GetString("tracker", cmdLine.GetString("tracking"));
	
	if( !useDefault )
		type = TypeFromStr(typeStr);
	
	if( type == KLT )
	{
	#if HAS_VPI
		return objectTrackerKLT::Create(cmdLine);
	#else
		LogError(LOG_TRACKER "couldn't create KLT object tracker (not built with VPI enabled)\n");
		return NULL;
	#endif
	}
	else if( type == IOU )
	{
		return objectTrackerIOU::Create(cmdLine);
	}
	else
	{
		if( typeStr != NULL )
			LogError(LOG_TRACKER "tried to create invalid object tracker type:  %s\n", typeStr);
	}
	
	return NULL;
}


// Create
objectTracker* objectTracker::Create( int argc, char** argv )
{
	return Create(commandLine(argc, argv));
}


// TypeToStr
const char* objectTracker::TypeToStr( objectTracker::Type type )
{
	if( type == IOU )
		return "IOU";
	else if( type == KLT )
		return "KLT";
	else
		return "none";
}


// TypeFromStr
objectTracker::Type objectTracker::TypeFromStr( const char* str )
{
	if( !str )
		return NONE;
	
	if( strcasecmp(str, "IOU") == 0 )
		return IOU;
	else if( strcasecmp(str, "KLT") == 0 )
		return KLT;
	else 
		return NONE;
}
