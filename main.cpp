#include <iostream>
#include <filesystem>
#include "scene-name-parser.hpp"

void print_help() {
    std::cout << "Usage:" << std::endl;
    std::cout << "parser [filename]" << std::endl;
}

int main(int argc, char *argv[]) {
    std::cout << "Scene name parser" << std::endl;
    if (argc == 1) {
        print_help();
    }
    if (argc == 2) {

        scene_name::scene_name_parser parser;
        std::pair<std::optional<scene_release_info::release_info>, scene_name::parsing_result> parse_result = parser.parse(
                argv[1]);
        if (parse_result.second == scene_name::parsing_result::pr_success) {
            std::cout << "Name: " << parse_result.first->name << std::endl;
            if (parse_result.first->year.has_value()) {
                std::cout << "Release year: " << parse_result.first->year.value() << std::endl;
            }
            if (parse_result.first->show_info.has_value()) {
                if (parse_result.first->show_info->complete_season) {
                    std::cout << "Complete season " << parse_result.first->show_info->season << std::endl;
                } else {
                    std::cout << "Season " << parse_result.first->show_info->season << ", Episode "
                              << parse_result.first->show_info->episode << std::endl;
                }
            }
            std::cout << "Release group: " << parse_result.first->group << std::endl;
            std::cout << "Release edition: ";
            switch (parse_result.first->edition_info) {

                case scene_release_info::scene_edition_info::ei_none:
                    std::cout << "Standard" << std::endl;
                    break;
                case scene_release_info::scene_edition_info::ei_extended:
                    std::cout << "Extended cut" << std::endl;
                    break;
                case scene_release_info::scene_edition_info::ei_directors_cut:
                    std::cout << "Directors cut" << std::endl;
                    break;
            }

            if (parse_result.first->media_info.language.has_value()) {
                std::cout << "Language: " << parse_result.first->media_info.language.value() << std::endl;
            }

            std::cout << "Container: ";
            switch (parse_result.first->media_info.container) {

                case scene_release_info::container_type::ct_unknown:
                    std::cout << "unknown" << std::endl;
                    break;
                case scene_release_info::container_type::ct_h264:
                    std::cout << "H.264 (AVC)" << std::endl;
                    break;
                case scene_release_info::container_type::ct_h265:
                    std::cout << "H.265 (HEVC)" << std::endl;
                    break;
            }

            std::cout << "Source: ";
            switch (parse_result.first->media_info.source) {

                case scene_release_info::media_source::ms_unknown:
                    std::cout << "unknown" << std::endl;
                    break;
                case scene_release_info::media_source::ms_bluray:
                    std::cout << "BluRay" << std::endl;
                    break;
                case scene_release_info::media_source::ms_web:
                    std::cout << "Web (Amazon/Netflix/etc)" << std::endl;
                    break;
                case scene_release_info::media_source::ms_r5:
                    std::cout << "Region5" << std::endl;
                    break;
                case scene_release_info::media_source::ms_ts:
                    std::cout << "Telescreen/CAM (yuck!)" << std::endl;
                    break;
            }

            std::cout << "Resolution: ";
            switch (parse_result.first->media_info.resolution) {

                case scene_release_info::resolution_info::ri_unknown:
                    std::cout << "unknown" << std::endl;
                    break;
                case scene_release_info::resolution_info::ri_1080:
                    std::cout << "FullHD/1080p" << std::endl;
                    break;
                case scene_release_info::resolution_info::ri_720:
                    std::cout << "720p" << std::endl;
                    break;
                case scene_release_info::resolution_info::ri_2160:
                    std::cout << "4K/UHD" << std::endl;
                    break;
            }

            std::cout << "Etc feature flags: ";
            for (auto &feature: parse_result.first->media_info.features) {
                std::cout << feature + " ";
            }
            std::cout << std::endl;
        }
    }
    return 0;
}