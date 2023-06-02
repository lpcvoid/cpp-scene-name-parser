#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "extern/doctest/doctest/doctest.h"
#include "scene-name-parser.hpp"



const std::map<std::string, scene_release_info::release_info> test_matrix_shows {
        /**
         * this is an evil one, since the movie name contains a number which may collide with the release date
         */
        {"Random.Show.Name.S03.Complete.German.DL.1080p.BluRay.x265-ReleaseGroup",
                {"Random Show Name",
                        2015,
                        scene_release_info::scene_release_type::rt_movie,
                        scene_release_info::scene_edition_info::ei_none,
                        scene_release_info::release_info_show{3, 0, true},
                        {
                                scene_release_info::container_type::ct_h265,
                                scene_release_info::resolution_info::ri_1080,
                                scene_release_info::media_source::ms_bluray,
                                "german",
                                {"DL"}
                        },
                        "ReleaseGroup"
                }
        },
        {"Random.Show.Episode.Name.S03E42.German.DL.1080p.BluRay.x265-ReleaseGroup",
         {"Random Show Episode Name",
          std::nullopt,
          scene_release_info::scene_release_type::rt_movie,
          scene_release_info::scene_edition_info::ei_none,
          scene_release_info::release_info_show{3, 42, false},
          {
                  scene_release_info::container_type::ct_h265,
                  scene_release_info::resolution_info::ri_1080,
                  scene_release_info::media_source::ms_bluray,
                  "german",
                  {"DL"}
          },
          "ReleaseGroup"
         }
        },
};

const std::map<std::string, scene_release_info::release_info> test_matrix_movies {
        /**
         * this is an evil one, since the movie name contains a number which may collide with the release date
         */
        {"Random.Movie.Name.666.2015.German.DL.1080p.BluRay.x265-ReleaseGroup",
         {"Random Movie Name 666",
          2015,
          scene_release_info::scene_release_type::rt_movie,
          scene_release_info::scene_edition_info::ei_none,
          std::nullopt,
          {
                  scene_release_info::container_type::ct_h265,
                  scene_release_info::resolution_info::ri_1080,
                  scene_release_info::media_source::ms_bluray,
                  "german",
                  {"DL"}
          },
          "ReleaseGroup"
         }
        },

        {"Random.Movie.Name.2015.German.DL.1080p.BluRay.x265-ReleaseGroup",
         {"Random Movie Name",
          2015,
          scene_release_info::scene_release_type::rt_movie,
          scene_release_info::scene_edition_info::ei_none,
          std::nullopt,
          {
                  scene_release_info::container_type::ct_h265,
                  scene_release_info::resolution_info::ri_1080,
                  scene_release_info::media_source::ms_bluray,
                  "german",
                  {"DL"}
          },
          "ReleaseGroup"
         }
        },
        {"Random Movie Name EXTENDED CUT German DL 1080p BluRay x265-ReleaseGroup",
         {"Random Movie Name DL",
          std::nullopt,
          scene_release_info::scene_release_type::rt_movie,
          scene_release_info::scene_edition_info::ei_extended,
          std::nullopt,
          {
                  scene_release_info::container_type::ct_h265,
                  scene_release_info::resolution_info::ri_1080,
                  scene_release_info::media_source::ms_bluray,
                  "german",
                  {"DL", "Random", "Movie", "Name"}
          },
          "ReleaseGroup"
         }
        },
        {"Random Movie Name EXTENDED CUT German 1080p BluRay x265-ReleaseGroup",
                {"Random Movie Name",
                        std::nullopt,
                        scene_release_info::scene_release_type::rt_movie,
                        scene_release_info::scene_edition_info::ei_extended,
                        std::nullopt,
                        {
                                scene_release_info::container_type::ct_h265,
                                scene_release_info::resolution_info::ri_1080,
                                scene_release_info::media_source::ms_bluray,
                                "german",
                                { "Random", "Movie", "Name"}
                        },
                        "ReleaseGroup"
                }
        },
        {"Random Movie Name 666 2022 DIRECTORS CUT German DD71 2160p DV DL HDR10 WebUHD x265-ReleaseGroup",
         {"Random Movie Name 666",
          2022,
          scene_release_info::scene_release_type::rt_movie,
          scene_release_info::scene_edition_info::ei_directors_cut,
          std::nullopt,
          {
                  scene_release_info::container_type::ct_h265,
                  scene_release_info::resolution_info::ri_2160,
                  scene_release_info::media_source::ms_web,
                  "german",
                  {"DL", "DV", "DD71", "HDR10"}
          },
          "ReleaseGroup"
         }
        },
        /**
         * Same as above, but with "DIRECTOR CUT", missing the "s"
         */
        {"Random Movie Name 2022 DIRECTOR CUT German DD71 2160p DV DL HDR10 WebUHD x265-ReleaseGroup",
         {"Random Movie Name",
          2022,
          scene_release_info::scene_release_type::rt_movie,
          scene_release_info::scene_edition_info::ei_directors_cut,
          std::nullopt,
          {
                  scene_release_info::container_type::ct_h265,
                  scene_release_info::resolution_info::ri_2160,
                  scene_release_info::media_source::ms_web,
                  "german",
                  {"DL", "DV", "DD71", "HDR10"}
          },
          "ReleaseGroup"
         }
        },

};

TEST_CASE("Scene name tests - movies"){

    scene_name::scene_name_parser parser;
    for (auto& test_case : test_matrix_movies) {
        auto parsing_result = parser.parse(test_case.first, scene_release_info::scene_release_type::rt_movie);
        CHECK_EQ(parsing_result.second, scene_name::parsing_result::pr_success);

        CHECK_EQ(parsing_result.first->release_type, test_case.second.release_type);
        CHECK_EQ(parsing_result.first->name, test_case.second.name);
        CHECK_EQ(parsing_result.first->year, test_case.second.year);
        CHECK_EQ(parsing_result.first->group, test_case.second.group);
        CHECK_EQ(parsing_result.first->show_info, std::nullopt); //no show info for movies!
        CHECK_EQ(parsing_result.first->edition_info, test_case.second.edition_info);
        //media info tests
        CHECK_EQ(parsing_result.first->media_info.container, test_case.second.media_info.container);
        CHECK_EQ(parsing_result.first->media_info.features, test_case.second.media_info.features);
        CHECK_EQ(parsing_result.first->media_info.language, test_case.second.media_info.language);
        CHECK_EQ(parsing_result.first->media_info.resolution, test_case.second.media_info.resolution);
        CHECK_EQ(parsing_result.first->media_info.source, test_case.second.media_info.source);
    }
}

TEST_CASE("Scene name tests - shows"){

    scene_name::scene_name_parser parser;
    for (auto& test_case : test_matrix_shows) {
        auto parsing_result = parser.parse(test_case.first, scene_release_info::scene_release_type::rt_show);
        CHECK_EQ(parsing_result.second, scene_name::parsing_result::pr_success);

        CHECK_EQ(parsing_result.first->release_type, test_case.second.release_type);
        CHECK_EQ(parsing_result.first->name, test_case.second.name);
        CHECK_EQ(parsing_result.first->year, test_case.second.year);
        CHECK_EQ(parsing_result.first->group, test_case.second.group);
        CHECK_EQ(parsing_result.first->show_info->episode, test_case.second.show_info->episode);
        CHECK_EQ(parsing_result.first->show_info->season, test_case.second.show_info->season);
        CHECK_EQ(parsing_result.first->show_info->complete_season, test_case.second.show_info->complete_season);
        CHECK_EQ(parsing_result.first->edition_info, test_case.second.edition_info);
        //media info tests
        CHECK_EQ(parsing_result.first->media_info.container, test_case.second.media_info.container);
        CHECK_EQ(parsing_result.first->media_info.features, test_case.second.media_info.features);
        CHECK_EQ(parsing_result.first->media_info.language, test_case.second.media_info.language);
        CHECK_EQ(parsing_result.first->media_info.resolution, test_case.second.media_info.resolution);
        CHECK_EQ(parsing_result.first->media_info.source, test_case.second.media_info.source);
    }
}