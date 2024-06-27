idf_component_register(SRCS "main.cpp" "speaker/pdm_player.cpp" "wav_file/wav_file.cpp"
                    INCLUDE_DIRS "." "speaker" "wav_file"
                    EMBED_FILES "../components/embedded_files/example.wav")


# target_add_binary_data(${COMPONENT_TARGET} "../components/embedded_files/example.wav" TEXT)