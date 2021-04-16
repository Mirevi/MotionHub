#pragma once

#define OUTPUT_BUFFER_SIZE 2048

static const char* LOCALHOST = "127.0.0.1";
static const char* BROADCAST = "172.16.255.255";

static const int DEFAULT_PORT = 7000;
static const char* DEFAULT_URI = "/mh/skeleton/";

static const char* IPV4_REGEX = "^((\\d|[1-9]\\d|[0-1]\\d{2}|2[0-4]\\d|25[0-5])[\\.]){3}(\\d|[1-9]\\d|[0-1]\\d{2}|2[0-4]\\d|25[0-5])$";
static const char* IPV6_REGEX = "^(?:[A-F0-9]{1,4}:){7}[A-F0-9]{1,4}$"; // Does not match shortened Addresses ::1 OR ::