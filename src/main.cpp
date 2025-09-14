#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

void OpenLog() {
    auto path = SKSE::log::log_directory();

    if (!path)
        return;

    *path /= "skyrim-rendering.log";

    std::vector<spdlog::sink_ptr> sinks{
        std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true),
        std::make_shared<spdlog::sinks::msvc_sink_mt>()
    };

    auto logger = std::make_shared<spdlog::logger>("global", sinks.begin(), sinks.end());

#ifndef NDEBUG
    logger->set_level(spdlog::level::debug);
    logger->flush_on(spdlog::level::debug);
#else
    logger->set_level(spdlog::level::info);
    logger->flush_on(spdlog::level::info);
#endif

    spdlog::set_default_logger(std::move(logger));
    spdlog::set_pattern("[%Y-%m-%d %T.%e][%-16s:%-4#][%L]: %v");
}

extern "C" __declspec(dllexport) void __stdcall Initialize()
{
    OpenLog();

    logger::info("skyrim-rendering v{}.{}.{} PreLoad"sv, Version::MAJOR, Version::MINOR, Version::PATCH);

}

extern "C" __declspec(dllexport) constinit auto SKSEPlugin_Version = []() {
    SKSE::PluginVersionData v;
    v.PluginVersion(Version::MAJOR);
    v.PluginName(Version::PROJECT);
    v.AuthorName("aers");
    v.UsesAddressLibrary();
    v.UsesUpdatedStructs();
    v.CompatibleVersions({ SKSE::RUNTIME_SSE_1_6_1170 });

    return v;
}();

extern "C" __declspec(dllexport) bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse, false);

	return true;
}
