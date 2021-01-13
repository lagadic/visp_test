project(test-tracking-model-based)

cmake_minimum_required(VERSION 2.6)

find_package(VISP REQUIRED visp_core visp_mbt visp_sensor)

# set the list of source files
set(test_cpp
  test-tracking-mb.cpp)

foreach(cpp ${test_cpp})
  visp_add_target(${cpp})
endforeach()

OPTION(disable_display "Turn OFF to enable display during testing" TRUE)

# triplet: video + model + tracker (all|edge|klt|hybrid)
list(APPEND video_model_list "teabox" "teabox" "all")
list(APPEND video_model_list "square-and-circle1" "square-and-circle1" "all")
list(APPEND video_model_list "square-and-circle2" "square-and-circle2" "all")
list(APPEND video_model_list "square-and-circle3" "square-and-circle2" "all")
list(APPEND video_model_list "cube" "cube" "all")
list(APPEND video_model_list "cube" "cube-and-cylinder" "all")
list(APPEND video_model_list "ravioli-box" "ravioli-box" "klt")
list(APPEND video_model_list "ravioli-box" "ravioli-box" "hybrid")
list(APPEND video_model_list "ravioli-box" "ravioli-box-rotx" "klt")
list(APPEND video_model_list "ravioli-box" "ravioli-box-rotx" "hybrid")
list(APPEND video_model_list "ravioli-box" "ravioli-box-circle2" "hybrid")
list(APPEND video_model_list "coke" "coke" "klt")
list(APPEND video_model_list "coke" "coke" "hybrid")
list(APPEND video_model_list "coke-circle" "coke-circle1" "hybrid")
list(APPEND video_model_list "coke-circle" "coke-circle2" "hybrid")
list(APPEND video_model_list "tabasco-box" "tabasco-box" "hybrid")

list(LENGTH video_model_list list_length)
MATH(EXPR list_size ${list_length}-1)

foreach(video_index RANGE 0 ${list_size} 3)
  MATH(EXPR model_index ${video_index}+1)
  MATH(EXPR track_index ${video_index}+2)
  list(GET video_model_list ${video_index} video)
  list(GET video_model_list ${model_index} model)
  list(GET video_model_list ${track_index} track)
  
  # loop over the trackers: (0) edges, (1) keypoint, (2) hybrid
  foreach(tracker RANGE 0 2 1)
    # loop over the visibility test: (0) none, (1) ogre, (2) scanline, (3) ogre+scanline
    foreach(visibility RANGE 0 3 1)
      # loop over optimization method: (0) gauss newton, (1) levenberg marquart
      foreach(optimization RANGE 0 1 1)
        if(NOT track MATCHES "all")
          if(NOT tracker EQUAL 0 AND track MATCHES "edge")
            break()
          elseif(NOT tracker EQUAL 1 AND track MATCHES "klt")
            break()
          elseif(NOT tracker EQUAL 2 AND track MATCHES "hybrid")
            break()
          endif()
        endif()
        set(test_name test-tracking-mb)
        set(bin_name  test-tracking-mb)
        set(bin_option "")

        set(test_name "${test_name}-video=${video}")
        list(APPEND bin_option --video ${video})
        set(test_name "${test_name}-model=${model}")
        list(APPEND bin_option --model ${model})
        set(test_name "${test_name}-tracker=${tracker}")
        list(APPEND bin_option --tracker ${tracker})
        set(test_name "${test_name}-visibility=${visibility}")
        list(APPEND bin_option --visibility ${visibility})
        set(test_name "${test_name}-optim=${optimization}")
        list(APPEND bin_option --optim ${optimization})

        list(APPEND bin_option --test-pose)
        if (disable_display)
          list(APPEND bin_option --turn-off-display)
        endif()
        #message("cmd: ${bin_name} ${bin_option}")
        add_test(NAME ${test_name} COMMAND ${bin_name} ${bin_option})
	
      endforeach()
    endforeach()
  endforeach()

endforeach()
