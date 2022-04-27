// Copyright (c) 2022 Sentry. All Rights Reserved.

#include "SentrySubsystemDesktop.h"
#include "SentryEventDesktop.h"
#include "SentryUserDesktop.h"
#include "SentryDefines.h"

#include "SentrySettings.h"
#include "SentryEvent.h"
#include "SentryUserFeedback.h"
#include "SentryUser.h"
#include "SentryModule.h"

#include "Convenience/SentryInclude.h"
#include "Infrastructure/SentryConvertorsDesktop.h"

#include "Misc/Paths.h"

void SentrySubsystemDesktop::InitWithSettings(const USentrySettings* settings)
{
#if PLATFORM_WINDOWS
	const FString HandlerExecutableName = TEXT("crashpad_handler.exe");
#elif PLATFORM_MAC
	const FString HandlerExecutableName = TEXT("crashpad_handler");
#endif

	const FString HandlerPath = FPaths::Combine(FSentryModule::Get().GetBinariesPath(), HandlerExecutableName);

	sentry_options_t* options = sentry_options_new();
	sentry_options_set_dsn(options, TCHAR_TO_ANSI(*settings->DsnUrl));
	sentry_options_set_handler_path(options, TCHAR_TO_ANSI(*HandlerPath));
	sentry_init(options);
}

void SentrySubsystemDesktop::Close()
{
	sentry_close();
}

void SentrySubsystemDesktop::AddBreadcrumb(const FString& message, const FString& category, const FString& type, const TMap<FString, FString>& data, ESentryLevel level)
{
	sentry_value_t sentryBreadcrumb = sentry_value_new_breadcrumb(TCHAR_TO_ANSI(*type), TCHAR_TO_ANSI(*message));

	if (!category.IsEmpty())
		sentry_value_set_by_key(sentryBreadcrumb, "category", sentry_value_new_string(TCHAR_TO_ANSI(*category)));

	FString levelStr = SentryConvertorsDesktop::SentryLevelToString(level);
	if (!levelStr.IsEmpty())
		sentry_value_set_by_key(sentryBreadcrumb, "level", sentry_value_new_string(TCHAR_TO_ANSI(*levelStr)));

	sentry_value_set_by_key(sentryBreadcrumb, "data", SentryConvertorsDesktop::StringMapToNative(data));

	sentry_add_breadcrumb(sentryBreadcrumb);
}

USentryId* SentrySubsystemDesktop::CaptureMessage(const FString& message, ESentryLevel level)
{
	sentry_value_t sentryEvent = sentry_value_new_message_event(SentryConvertorsDesktop::SentryLevelToNative(level), nullptr, TCHAR_TO_ANSI(*message));
	sentry_uuid_t id = sentry_capture_event(sentryEvent);
	return SentryConvertorsDesktop::SentryIdToUnreal(id);
}

USentryId* SentrySubsystemDesktop::CaptureMessageWithScope(const FString& message, const FConfigureScopeDelegate& onScopeConfigure, ESentryLevel level)
{
	UE_LOG(LogSentrySdk, Log, TEXT("CaptureMessageWithScope method is not supported for the current platform."));
	return nullptr;
}

USentryId* SentrySubsystemDesktop::CaptureEvent(USentryEvent* event)
{
	TSharedPtr<SentryEventDesktop> eventDesktop = StaticCastSharedPtr<SentryEventDesktop>(event->GetNativeImpl());

	sentry_uuid_t id = sentry_capture_event(eventDesktop->GetNativeObject());
	return SentryConvertorsDesktop::SentryIdToUnreal(id);
}

USentryId* SentrySubsystemDesktop::CaptureEventWithScope(USentryEvent* event, const FConfigureScopeDelegate& onScopeConfigure)
{
	UE_LOG(LogSentrySdk, Log, TEXT("CaptureEventWithScope method is not supported for the current platform."));
	return nullptr;
}

void SentrySubsystemDesktop::CaptureUserFeedback(USentryUserFeedback* userFeedback)
{
	UE_LOG(LogSentrySdk, Log, TEXT("CaptureUserFeedback method is not supported for the current platform."));
}

void SentrySubsystemDesktop::SetUser(USentryUser* user)
{
	TSharedPtr<SentryUserDesktop> userDesktop = StaticCastSharedPtr<SentryUserDesktop>(user->GetNativeImpl());
	sentry_set_user(userDesktop->GetNativeObject());
}

void SentrySubsystemDesktop::RemoveUser()
{
	sentry_remove_user();
}

void SentrySubsystemDesktop::ConfigureScope(const FConfigureScopeDelegate& onConfigureScope)
{
	UE_LOG(LogSentrySdk, Log, TEXT("CaptureUserFeedback method is not supported for the current platform."));
}

void SentrySubsystemDesktop::SetContext(const FString& key, const TMap<FString, FString>& values)
{
	sentry_set_context(TCHAR_TO_ANSI(*key), SentryConvertorsDesktop::StringMapToNative(values));
}

void SentrySubsystemDesktop::SetTag(const FString& key, const FString& value)
{
	sentry_set_tag(TCHAR_TO_ANSI(*key), TCHAR_TO_ANSI(*value));
}

void SentrySubsystemDesktop::RemoveTag(const FString& key)
{
	sentry_remove_tag(TCHAR_TO_ANSI(*key));
}

void SentrySubsystemDesktop::SetLevel(ESentryLevel level)
{
	sentry_set_level(SentryConvertorsDesktop::SentryLevelToNative(level));
}