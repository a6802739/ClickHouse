#pragma once
/// Вспомогательные определения облегчающие работу с PoCo logging.

#include <sstream>
#include <Poco/Logger.h>

#ifndef QUERY_PREVIEW_LENGTH
#define QUERY_PREVIEW_LENGTH 160
#endif

using Poco::Logger;

/// Logs a message to a specified logger with that level.

#define LOG_TRACE(logger, message) do { \
	if ((logger)->trace()) {\
	std::stringstream oss;	\
	oss << message; \
	(logger)->trace(oss.str());}} while(0)

#define LOG_DEBUG(logger, message) do { \
	if ((logger)->debug()) {\
	std::stringstream oss;	\
	oss << message; \
	(logger)->debug(oss.str());}} while(0)

#define LOG_INFO(logger, message) do { \
	if ((logger)->information()) {\
	std::stringstream oss;	\
	oss << message; \
	(logger)->information(oss.str());}} while(0)

#define LOG_NOTICE(logger, message) do { \
	if ((logger)->notice()) {\
	std::stringstream oss;	\
	oss << message; \
	(logger)->notice(oss.str());}} while(0)

#define LOG_WARNING(logger, message) do { \
	if ((logger)->warning()) {\
	std::stringstream oss;	\
	oss << message; \
	(logger)->warning(oss.str());}} while(0)

#define LOG_ERROR(logger, message) do { \
	if ((logger)->error()) {\
	std::stringstream oss;	\
	oss << message; \
	(logger)->error(oss.str());}} while(0)

#define LOG_CRITICAL(logger, message) do { \
	if ((logger)->critical()) {\
	std::stringstream oss;	\
	oss << message; \
	(logger)->critical(oss.str());}} while(0)

#define LOG_FATAL(logger, message) do { \
	if ((logger)->fatal()) {\
	std::stringstream oss;	\
	oss << message; \
	(logger)->fatal(oss.str());}} while(0)