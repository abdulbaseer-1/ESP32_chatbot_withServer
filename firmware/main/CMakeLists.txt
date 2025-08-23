idf_component_register(
                SRCS "main.c"
                PRIV_REQUIRES spi_flash custom_audio custom_network custom_system custom_utils
                INCLUDE_DIRS ".")
