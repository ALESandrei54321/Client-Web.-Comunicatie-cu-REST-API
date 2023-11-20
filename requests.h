#ifndef _REQUESTS_
#define _REQUESTS_
#include <string>
#include "json.hpp"
#include <string>
#include <iostream>
#include <bits/stdc++.h>
#define MAX_COOKIES 1000

using namespace std; 
// computes and returns a GET request string (query_params
// and cookies can be set to NULL if not needed)
char *compute_get_request(char *host, char *url, char *query_params,
							 string cookies[MAX_COOKIES], int cookies_count);

// computes and returns a POST request string (cookies can be NULL if not needed)
char *compute_post_request(char *host, char *url, char* content_type, nlohmann::json body_data,
                            int body_data_fields_count, string cookies[MAX_COOKIES], int cookies_count);

char *compute_delete_request(char *host, char *url, char *query_params, string cookies[MAX_COOKIES], 
                            int cookies_count);
#endif
