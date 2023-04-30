#include "pch.h"
#include "Request.h"

#include <curl/curl.h>

#ifndef _DEBUG
#include <ThemidaSDK.h>
#endif

inline size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

std::string Request::Perform(char* requestBody, size_t requestBodySize)
{
#ifndef _DEBUG
	VM_PUMA_WHITE_START
#endif 
		auto curl = curl_easy_init();
	std::string readBuffer;

	CURLcode res;
	if (curl) {
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Host: dopp.in");

		curl_easy_setopt(curl, CURLOPT_URL, m_Link);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, requestBodySize);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NO_REVOKE);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		curl_slist_free_all(headers);
	}

#ifndef _DEBUG
	VM_PUMA_WHITE_END
#endif 
		return readBuffer;
}