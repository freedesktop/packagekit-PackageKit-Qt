set(kpk_addrm_SRCS
    kpk-addrm.cpp
    PkAddRm.cpp
    PkAddRm_Model.cpp
    PkDelegate.cpp
    PkReviewChanges.cpp
    ../Common/PkTransaction.cpp
    PkRequirements.cpp
    ../Common/PkStrings.cpp
)

kde4_add_ui_files(kpk_addrm_SRCS
    PkAddRm.ui
    PkReviewChanges.ui
    ../Common/PkTransaction.ui
    PkRequirements.ui
)

kde4_add_plugin(kcm_kpk_addrm ${kpk_addrm_SRCS})
target_link_libraries(kcm_kpk_addrm ${KDE4_KDEUI_LIBS} ${QPACKAGEKIT_LIB})

install(TARGETS kcm_kpk_addrm  DESTINATION ${PLUGIN_INSTALL_DIR} )
