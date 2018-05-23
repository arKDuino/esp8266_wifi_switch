all:
	cp ../sketch/page_generator.h page_generator.h
	cp ../sketch/constants_lang_pt.h constants_lang_pt.h
	g++ test.cpp -o tests.exe
	tests.exe

clean:
	rm page_generator.h
	rm constants_lang_pt.h
	rm -f tests.exe
	rm -f configPage.html
	rm -f configurationSaved.html
	rm -f switchPage.html
	rm -f ConfigurationInvalidPASSWD.html
	rm -f ConfigurationInvalidSSID.html
	rm -f ConfigurationInvalidIP.html