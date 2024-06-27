idf_component_register(SRCS "main.cpp" "speaker/pdm_player.cpp"
                    INCLUDE_DIRS "." "speaker"
                    EMBED_FILES "../components/embedded_files/example.wav")


# target_add_binary_data(${COMPONENT_TARGET} "../components/embedded_files/example.wav" TEXT)