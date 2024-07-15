idf_component_register(SRCS "main.cpp" "speaker/pdm_player.cpp" "wav_file/wav_file_reader.cpp" "mp3_file/mp3_file_reader.cpp" 
                    INCLUDE_DIRS "." "speaker" "wav_file" "mp3_file"
                    EMBED_FILES "../components/embedded_files/test.wav"
                    EMBED_FILES "../components/embedded_files/test.mp3"
)


# target_add_binary_data(${COMPONENT_TARGET} "../components/embedded_files/example.wav" TEXT)