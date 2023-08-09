QDBUSXML2CPP_ADAPTOR_HEADER = carinformation_adaptor.h
QDBUSXML2CPP_ADAPTOR_SOURCE = carinformation_adaptor.cpp
QDBUSXML2CPP_ADAPTOR_FLAGS = -a

adaptor.target = $$QDBUSXML2CPP_ADAPTOR_HEADER $$QDBUSXML2CPP_ADAPTOR_SOURCE
adaptor.commands = qdbusxml2cpp $$QDBUSXML2CPP_ADAPTOR_FLAGS -c CarInformationAdaptor -l CarInformation -i carinformation.h carinformation.xml -o $$QDBUSXML2CPP_ADAPTOR_HEADER && \
                   qdbusxml2cpp $$QDBUSXML2CPP_ADAPTOR_FLAGS -c CarInformationAdaptor -l CarInformation -i carinformation.h carinformation.xml -o $$QDBUSXML2CPP_ADAPTOR_SOURCE
adaptor.depends = carinformation.xml
adaptor.CONFIG = no_link target_predeps
QMAKE_EXTRA_TARGETS += adaptor

PRE_TARGETDEPS += $$QDBUSXML2CPP_ADAPTOR_HEADER $$QDBUSXML2CPP_ADAPTOR_SOURCE

HEADERS += $$QDBUSXML2CPP_ADAPTOR_HEADER
SOURCES += $$QDBUSXML2CPP_ADAPTOR_SOURCE