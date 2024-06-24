idf_component_register(SRCS "main.cpp" "Speaker/PDM_Player.cpp"
                    INCLUDE_DIRS "." "Speaker"
                    EMBED_FILES "../components/embedded_files/example.wav")


# target_add_binary_data(${COMPONENT_TARGET} "../components/embedded_files/example.wav" TEXT)