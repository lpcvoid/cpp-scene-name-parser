#pragma once

#include <string>
#include <optional>
#include <utility>
#include <vector>
#include <map>
#include <sstream>
#include <regex>

namespace scene_release_info {

    const std::array<std::string, 3> ALLOWED_DELIMITERS{
            " ", ".", "_"
    };


    enum class scene_release_type {
        rt_unknown = 0,
        rt_movie = 1,
        rt_show = 2
    };

    /**
     * Container type of the release
     */
    enum class container_type {
        ct_unknown = 0,
        ct_h264 = 1,
        ct_h265 = 2 //HVEC
    };

    enum class resolution_info {
        ri_unknown = 0,
        ri_1080 = 1,
        ri_720 = 2,
        ri_2160 = 3 // UHD, 4K...
    };

    enum class media_source {
        ms_unknown = 0,
        ms_bluray = 1,
        ms_web = 2,
        ms_r5 = 3,
        ms_ts = 4 //might as well name this ms_trash
    };

    enum class scene_edition_info {
        ei_none = 0,
        ei_extended = 1,
        ei_directors_cut = 2
    };

    struct release_media_info {
        container_type container = container_type::ct_unknown;
        resolution_info resolution = resolution_info::ri_unknown;
        media_source source = media_source::ms_unknown;
        std::optional<std::string> language;
        std::set<std::string> features;
    };

    struct release_info_show {
        uint16_t season = 0;
        uint16_t episode = 0;
        bool complete_season = false;
    };

    struct release_info {
        std::string name;
        std::optional<uint16_t> year = 0;
        scene_release_type release_type = scene_release_type::rt_unknown;
        scene_edition_info edition_info = scene_edition_info::ei_none;
        std::optional<release_info_show> show_info;
        release_media_info media_info;
        std::string group;
    };

}

namespace scene_name {

    enum class parsing_result {
        pr_success = 0,
        pr_empty_name,
        pr_malformed,
        pr_no_delimiter
    };

    class scene_name_parser {
    private:
        std::string _delimiter; //no delimiter means we guess from the passed name
        std::regex _season_regex;
        std::regex _episode_regex;

        // stole from https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c/46931770#46931770
        // because I am a lazy bugger
        static std::vector<std::string> split(const std::string &s, const std::string &delimiter) {
            size_t pos_start = 0, pos_end, delim_len = delimiter.length();
            std::string token;
            std::vector<std::string> res;

            while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
                token = s.substr(pos_start, pos_end - pos_start);
                pos_start = pos_end + delim_len;
                res.push_back(token);
            }

            res.push_back(s.substr(pos_start));
            return res;
        }

        static std::size_t count_occurences(const std::string &str, std::string substr) {
            std::size_t c = 0;
            std::size_t pos = 0;
            while ((pos = str.find(substr, pos)) != std::string::npos) {
                c++;
                pos += substr.length();
            }
            return c;
        }

        /**
         * Given a release name string, figure out the probable delimiter.
         *
         * Works by just checking which of the allowed delimiters occurs most
         * often within the release name.
         *
         * @param release_name
         * @return the guessed delimiter
         */
        static std::string guess_delimiter(const std::string &release_name) {
            std::vector<std::pair<std::string, std::size_t>> delimiter_stats;
            delimiter_stats.reserve(scene_release_info::ALLOWED_DELIMITERS.size());
            for (const auto &delim: scene_release_info::ALLOWED_DELIMITERS) {
                delimiter_stats.emplace_back(delim, count_occurences(release_name, delim));
            }

            auto f = [](std::pair<std::string, std::size_t> const &a, std::pair<std::string, std::size_t> const &b) {
                return (a.second > b.second);
            };

            std::sort(delimiter_stats.begin(), delimiter_stats.end(), f);

            return delimiter_stats.front().first;
        }

        /**
         * Checks if a string consists of 0123456789
         * Ignores
         * @param str the string to test
         * @return if a string only consists of the mentioned characters
         */
        static bool is_numeric(const std::string &str) {
            return std::find_if(str.begin(), str.end(), [](char c) {
                return !std::isdigit(c);
            }) == str.end();
        }

        static std::string to_lowercase(std::string str) {
            std::transform(str.begin(), str.end(), str.begin(), ::tolower);
            return str;
        }

        /**
         * checks if a given feature exists within the string of release name parts.
         * Ignores case.
         * @param release_name_parts the release name, in parts
         * @param feature keyword
         * @param accept_substring the feature will be treated as substrings for matching
         * @param remove_if_matched if found, removes the feature from @param release_name_parts
         * @return
         */
        static bool check_feature(std::vector<std::string> &release_name_parts, const std::string &feature,
                                  bool accept_substring = false, bool remove_if_matched = false) {


            auto comparator = [&](const std::string &part, const std::string &feature) -> bool {
                return (accept_substring) ? (count_occurences(part, feature) > 0) : (part == feature);
            };

            if (feature.empty()) {
                return false;
            } else {
                std::string lower_feature = to_lowercase(feature);
                for (const std::string &release_name_part: release_name_parts) {
                    if (comparator(to_lowercase(release_name_part), lower_feature)) {
                        if (remove_if_matched) {
                            release_name_parts.erase(std::remove(release_name_parts.begin(), release_name_parts.end(), release_name_part), release_name_parts.end());


                        }
                        return true;
                    }
                }
            }

            return false;
        }

        // trim from start (in place)
        static inline void ltrim(std::string &s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
        }

        // trim from end (in place)
        static inline void rtrim(std::string &s) {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(), s.end());
        }

        // trim from both ends (in place)
        static inline void trim(std::string &s) {
            rtrim(s);
            ltrim(s);
        }



    public:
        explicit scene_name_parser(std::string delimiter = "") {
            _delimiter = std::move(delimiter);
            _season_regex = std::regex("^[Ss]([0-9]{1,2})");
            _episode_regex = std::regex("[Ee]([0-9]{1,2})$");
        }

        inline std::pair<std::optional<scene_release_info::release_info>, parsing_result>
        parse(const std::string &release_name, scene_release_info::scene_release_type release_type = scene_release_info::scene_release_type::rt_unknown) {

            scene_release_info::release_info ri;

            if (release_name.empty()) {
                return {std::nullopt, parsing_result::pr_empty_name};
            }

            std::string local_delimiter;

            local_delimiter = (_delimiter.empty() ? guess_delimiter(release_name) : _delimiter);

            std::cout << local_delimiter;

            //split up the release name by the delimiter

            std::vector<std::string> release_name_parts = split(release_name, local_delimiter);

            /*
             * we start on the left, by adding each string to the title, until we reach a number.
             * the number signals the release year. The title can also include numbers though, so once we found
             * a number, we will check if the next is also one, and use the current string as part of the title
             * instead.
             *
             * Random.Movie.Name.2000.2023.German.DL.1080p.BluRay.x265-ReleaseGroup
             *
             * This would mean the title is "Random Movie Name 2000" and the release year is 2023.
             *
             * Some releases also omit the release date! In this case, we can have a worst case release named like
             * this:
             *
             * Random.Movie.Name.German.DL.1080p.BluRay.x265-ReleaseGroup
             */

            while (!release_name_parts.empty()) {
                if (!is_numeric(release_name_parts.front())) {
                    ri.name += release_name_parts.front() + " ";
                    release_name_parts.erase(release_name_parts.begin());
                } else {
                    /*
                     * we encountered a number.
                     * We check if the next value is also a number (see above).
                     */

                    if (release_name_parts.size() >= 2 && is_numeric(*(release_name_parts.begin() + 1))) {
                        //yes, next part is also a number - this number is a part of the title.
                        ri.name += release_name_parts.front();
                        release_name_parts.erase(release_name_parts.begin());
                    } else {
                        //no, next part is not a number - we found the year
                        ri.year = std::stol(release_name_parts.front());
                        release_name_parts.erase(release_name_parts.begin());
                        break;
                    }

                }
            }

            trim(ri.name);

            /*
             * if the parsed name is now the same size as the release name parts, then the function above
             * simply chewed through all the feature words. It means that the release doesn't contain a release year -
             * this is probably the most annoying case of malformed release name, since we strongly depend on this year
             * to delimit the title from the source feature words.
             *
             * If this happens, we will set the year to null and go on as usual for now, processing (and removing) as many elements
             * of the release name as we can. In the end, we will use the remaining ones to construct a name -
             * this will most likely be less than perfect, though.
             */

            if (ri.name.size() == release_name.size()) {
                // split the parts again
                release_name_parts = split(release_name, local_delimiter);
                // set the year to unknown
                ri.year.reset();
                // reset name to null
                ri.name.clear();
            }


            /*
             * Next, we check for the group name. It should be at the very back of the release name,
             * seperated from the rest by a minus.
             */

            auto group_name_parts = split(release_name_parts.back(), "-");
            if (group_name_parts.size() == 2) {
                ri.group = group_name_parts.back();
                /* we found the group name, but need to add the first part back the the release name parts, but
                 * without the release group.
                 */
                release_name_parts.erase(release_name_parts.end());
                release_name_parts.push_back(group_name_parts.front());
            }

            /*
             * Now we should have something left like:
             *
             * DIRECTORS CUT German DD71 2160p DV DL HDR10 WebUHD x265-ReleaseGroup
             *
             * We will handle this by checking for various substrings within the parts.
             * This will be a collection of hand-written logic, and may break if somebody
             * names their releases in a very weird fashion. It will probably grow more robust over time.
             */

            /*
             * First we check for the edition.
             *
             */

            if (check_feature(release_name_parts, "DIRECTOR", true, true)) {
                ri.edition_info = scene_release_info::scene_edition_info::ei_directors_cut;
            }

            if (check_feature(release_name_parts, "EXTENDED", true, true)) {
                ri.edition_info = scene_release_info::scene_edition_info::ei_extended;
            }

            // remove "CUT" since it should only happen here
            release_name_parts.erase(std::remove(release_name_parts.begin(), release_name_parts.end(), "CUT"), release_name_parts.end());

            /*
             * Resolution
             */

            if (check_feature(release_name_parts, "2160", true, true) ||
                check_feature(release_name_parts, "UHD", false, true) ||
                check_feature(release_name_parts, "4K", false, true)) {
                ri.media_info.resolution = scene_release_info::resolution_info::ri_2160;
            }

            if (check_feature(release_name_parts, "1080", true, true) ||
                check_feature(release_name_parts, "FullHD", true, true)) {
                ri.media_info.resolution = scene_release_info::resolution_info::ri_1080;
            }

            if (check_feature(release_name_parts, "720", true, true)) {
                ri.media_info.resolution = scene_release_info::resolution_info::ri_720;
            }

            /*
             * Source
             */

            if (check_feature(release_name_parts, "BluRay", true, true) ||
                check_feature(release_name_parts, "BDRip", false, true)) {
                ri.media_info.source = scene_release_info::media_source::ms_bluray;
            }

            if (check_feature(release_name_parts, "R5", true, true) ||
                check_feature(release_name_parts, "Region5", false, true)) {
                ri.media_info.source = scene_release_info::media_source::ms_r5;
            }

            if (check_feature(release_name_parts, "Web", true, true) ||
                check_feature(release_name_parts, "Amazon", false, true) ||
                check_feature(release_name_parts, "Netflix", false, true)) {
                ri.media_info.source = scene_release_info::media_source::ms_web;
            }

            if (check_feature(release_name_parts, "TeleScreen", true, true) ||
                check_feature(release_name_parts, "TS", false, true) ||
                check_feature(release_name_parts, "CAM", false, true) ||
                check_feature(release_name_parts, "CamRip", false, true)) {
                ri.media_info.source = scene_release_info::media_source::ms_ts;
            }

            /*
             * Container
             */

            if (check_feature(release_name_parts, "265", true, true) ||
                check_feature(release_name_parts, "HEVC", true, true)) {
                ri.media_info.container = scene_release_info::container_type::ct_h265;
            }

            if (check_feature(release_name_parts, "264", true, true) ||
                check_feature(release_name_parts, "AVC", true, true)) {
                ri.media_info.container = scene_release_info::container_type::ct_h264;
            }

            /*
             * Languages
             *
             * I think this is most likely the most unfinished part. Will add new ones when I come accross them.
             * Important note - by default, the scene seems to assume english.
             */

            ri.media_info.language = "english";

            if (check_feature(release_name_parts, "GERMAN", true, true) ||
                check_feature(release_name_parts, "DEUTSCH", true, true)) {
                ri.media_info.language = "german";
            }

            /*
             * Now we can try and figure out if this is a show or a movie (or none, if we fail both), unless the user
             * supplied a type and is thus sure about this.
             *
             * Normally, within elements of a show, we can find episode or season indication strings, such as:
             *
             * S01E02 - 1. season, 2. episode
             * S01 - complete 1. season
             *
             * Rarely, this weird representation also exists:
             *
             * 01x02 - 1. season, 2. episode
             *
             * If we can't find any the Season, and we find indicators that the release has video elements,
             * we consider it as non-movie and non-show - maybe music or documentary.
             *
             * It's also important to do this at the end, since the cpp regex implementation isn't the best,
             * and it's the slowest operation by far here - so it makes sence to perform it on as little
             * feature strings as possible.
             */

            if (release_type == scene_release_info::scene_release_type::rt_unknown) {
                // user wants us to guess the release type

                for (auto& release_name_part : release_name_parts) {
                    std::smatch season_match;
                    std::smatch episode_match;
                    if (std::regex_match(release_name_part, season_match, _season_regex)) {
                        ri.release_type = scene_release_info::scene_release_type::rt_show;
                        ri.show_info = {};
                        ri.show_info->season =  std::stol(season_match.str());

                        // is it an episode or a complete season?
                        if (std::regex_match(release_name_part, episode_match, _episode_regex)) {
                            if (episode_match.size() == 2) {
                                std::ssub_match sub_match = episode_match[0];
                                ri.show_info->episode = std::stol(sub_match.str());
                            }
                        } else {
                            // we treat this release as a whole season
                            ri.show_info->complete_season = true;
                        }


                        release_name_parts.erase(std::remove(release_name_parts.begin(), release_name_parts.end(), release_name_part), release_name_parts.end());


                        break;
                    }
                }

            } else {
                ri.release_type = release_type;
            }


            /*
             * As explained above, we check if the release year was found correctly.
             * If not - we try and extract the title by concatenating what's left of the feature parts.
             */
            if (!ri.year.has_value()) {
                for (auto& release_name_part : release_name_parts) {
                    ri.name += release_name_part + " ";
                }
                trim(ri.name);
            }



            /*
             * remaining features
             */

            ri.media_info.features = {release_name_parts.begin(), release_name_parts.end()};


            return {ri, parsing_result::pr_success};
        };
    };
}




