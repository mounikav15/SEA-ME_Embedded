# D-Bus Adaptor generation
QDBUSXML2CPP_ADAPTOR_HEADER = carinformation_adaptor.h
QDBUSXML2CPP_ADAPTOR_SOURCE = carinformation_adaptor.cpp
QDBUSXML2CPP_ADAPTOR_FLAGS = -a

carinformation_adaptor.h: carinformation.xml
	qdbusxml2cpp $$QDBUSXML2CPP_ADAPTOR_FLAGS -c CarInformationAdaptor -l CarInformation -i carinformation.h carinformation.xml -o $$QDBUSXML2CPP_ADAPTOR_HEADER

carinformation_adaptor.cpp: carinformation.xml
	qdbusxml2cpp $$QDBUSXML2CPP_ADAPTOR_FLAGS -c CarInformationAdaptor -l CarInformation -i carinformation.h carinformation.xml -o $$QDBUSXML2CPP_ADAPTOR_SOURCE

HEADERS += $$QDBUSXML2CPP_ADAPTOR_HEADER
SOURCES += $$QDBUSXML2CPP_ADAPTOR_SOURCE