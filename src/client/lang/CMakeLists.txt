## SPDX-License-Identifier: MPL-2.0
## Copyright (c) 2025 Kirill Dmitrievich
## File: CMakeLists.txt; Created: Sat Nov 29 2025 00:35:20
## Description: Language translation files

set(TRANSLATION_FILES)
list(APPEND TRANSLATION_FILES prospero_en.ts)
list(APPEND TRANSLATION_FILES prospero_ru.ts)

qt_add_translations(prospero TS_FILES ${TRANSLATION_FILES} RESOURCE_PREFIX "/")
