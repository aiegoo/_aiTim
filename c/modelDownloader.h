/*uconcreative
 
#ifndef __MODEL_DOWNLOADER_H__
#define __MODEL_DOWNLOADER_H__


#include "json.hpp"


/**
 * Download a pre-trained model given its JSON config.
 * @ingroup modelDownloader
 */
bool DownloadModel( nlohmann::json& model, uint32_t retries=2 );


/**
 * Download a pre-trained model and return its JSON config.
 * @ingroup modelDownloader
 */
bool DownloadModel( const char* type, const char* name, nlohmann::json& model, uint32_t retries=2 );


/**
 * Check if a model can be found in the model manifest.
 * @ingroup modelDownloader
 */
bool FindModel( const char* type, const char* name );


/**
 * Find a pre-trained model's config in the model manifest.
 * @ingroup modelDownloader
 */
bool FindModel( const char* type, const char* name, nlohmann::json& model );


/**
 * Find a pre-trained model's config in the model manifest.
 * @ingroup modelDownloader
 */
bool FindModel( const char* type, const char* name, nlohmann::json& models, nlohmann::json& model );


/**
 * Load the model manifest from a JSON file.
 * @ingroup modelDownloader
 */
bool LoadModelManifest( const char* path="networks/models.json" );


/**
 * Load the model manifest from a JSON file.
 * @ingroup modelDownloader
 */
bool LoadModelManifest( nlohmann::json& models, const char* path="networks/models.json" );


/**
 * Helper macro for getting a string from a JSON element
 * @ingroup modelDownloader
 */
#define JSON_STR(x)					(x.is_string() ? x.get<std::string>() : "")


/**
 * Helper macro for getting a string from a JSON element
 * @ingroup modelDownloader
 */
#define JSON_STR_DEFAULT(x, default)	(x.is_string() ? x.get<std::string>() : default)


#endif