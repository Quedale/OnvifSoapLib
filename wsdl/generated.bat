rd /s/q %cd%\src\generated  
md %cd%\src\generated  
%cd%\subprojects\gsoap-2.8\gsoap\bin\win64\wsdl2h.exe -x -t %cd%\wsdl\typemap.dat -o %cd%\src\generated\common_service.h -c ^
http://www.onvif.org/onvif/ver10/device/wsdl/devicemgmt.wsdl ^
http://www.onvif.org/onvif/ver10/event/wsdl/event.wsdl ^
http://www.onvif.org/onvif/ver10/display.wsdl ^
http://www.onvif.org/onvif/ver10/deviceio.wsdl ^
http://www.onvif.org/onvif/ver20/imaging/wsdl/imaging.wsdl ^
http://www.onvif.org/onvif/ver10/media/wsdl/media.wsdl ^
http://www.onvif.org/onvif/ver20/ptz/wsdl/ptz.wsdl ^
http://www.onvif.org/onvif/ver10/receiver.wsdl ^
http://www.onvif.org/onvif/ver10/recording.wsdl ^
http://www.onvif.org/onvif/ver10/search.wsdl ^
http://www.onvif.org/onvif/ver10/replay.wsdl ^
http://www.onvif.org/onvif/ver20/analytics/wsdl/analytics.wsdl ^
http://www.onvif.org/onvif/ver10/analyticsdevice.wsdl ^
http://www.onvif.org/onvif/ver10/schema/onvif.xsd 
%cd%\subprojects\gsoap-2.8\gsoap\bin\win64\soapcpp2.exe -CL -x -I "%cd%\subprojects\gsoap-2.8\gsoap\import";"%cd%\subprojects\gsoap-2.8\gsoap" %cd%\src\generated\common_service.h -d%cd%\src\generated