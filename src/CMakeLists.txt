# Bombic source files

set(BOMBIC_SOURCES
    Config.cpp
    D3DXApp.cpp
    GBase.cpp
    GBomba.cpp
    GBombaMega.cpp
    GBombaNapalm.cpp
    GBomber.cpp
    GBonus.cpp
    GBonus_bomb.cpp
    GBonus_fire.cpp
    GBonus_fireman.cpp
    GBonus_kicker.cpp
    GBonus_live.cpp
    GBonus_megabomb.cpp
    GBonus_n_berserk.cpp
    GBonus_n_dement.cpp
    GBonus_n_kicker.cpp
    GBonus_n_nobomb.cpp
    GBonus_n_slow.cpp
    GBonus_n_stop.cpp
    GBonus_napalmbomb.cpp
    GBonus_nemoc_ostatni.cpp
    GBonus_posilani.cpp
    GBonus_shield.cpp
    GBonus_speed.cpp
    GBonus_timer.cpp
    GFile.cpp
    GGame.cpp
    GMap.cpp
    GMrcha.cpp
    GMrchaAI0.cpp
    GMrchaAI1.cpp
    GMrchaAI2.cpp
    GMrchaAI3.cpp
    GMrchaAI4.cpp
    GMrchaAI5.cpp
    GMrchaAI_BOSS_DUDLIK.cpp
    GMrchaAI_BOSS_TV.cpp
    GMrchaAI_Blesk.cpp
    GObject.cpp
    GView.cpp
    GZed.cpp
    HD3DX.cpp
    HDInput.cpp
    HDSound.cpp
    HFont.cpp
    HTranslate.cpp
    MainFrm.cpp
    MCredits.cpp
    MDeadMatch.cpp
    MDeadMatching.cpp
    MHelp.cpp
    MIntro.cpp
    MMain.cpp
    MMenu.cpp
    MMrchovnik.cpp
    MSetting.cpp
    MSingle.cpp
    MSinglePlaying.cpp
    searchfile.cpp
)

# Add network sources only if SDL2_net is available
if(HAVE_SDL2_NET)
    list(APPEND BOMBIC_SOURCES
        Network.cpp
        MLAN.cpp
        MLANHost.cpp
        MLANJoin.cpp
        MLANLobby.cpp
        MLANPlaying.cpp
    )
endif()

# Create executable
add_executable(bombic ${BOMBIC_SOURCES})

# Include directories
target_include_directories(bombic PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/..
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
)

if(HAVE_SDL2_NET)
    target_include_directories(bombic PRIVATE ${SDL2_NET_INCLUDE_DIRS})
endif()

# Link libraries
target_link_libraries(bombic PRIVATE
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)

if(HAVE_SDL2_NET)
    target_link_libraries(bombic PRIVATE ${SDL2_NET_LIBRARIES})
endif()

# Compile definitions
target_compile_definitions(bombic PRIVATE
    HAVE_CONFIG_H
)

if(HAVE_SDL2_NET)
    target_compile_definitions(bombic PRIVATE HAVE_SDL2_NET)
endif()

# Install executable
install(TARGETS bombic
    RUNTIME DESTINATION bin
)
