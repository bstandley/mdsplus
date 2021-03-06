PUBLIC FUN TSSLICE (IN _sig, IN _arg, IN _sys)

/* This function returns the Thomson Scattering density or temperature
   from the chord nearest to _arg

   _sig = \TSxx_yyyy

	where xx = "NE" or "TE"
	and yyyy = "CORE" or "DIV"

   _sys = "DIV" or "CORE"

   Created: 98.07.21 Jeff Schachter
*/

{
	_z = DATA(BUILD_PATH("\\TSZ_"//_sys))*100.;  
	_delta = ABS(_z - FLOAT(_arg));
	_index = MINLOC(_delta);

	
        _array = DATA(_s = BUILD_PATH("\\"//_sig))[*,_index[0]];
	_d = DIM_OF(_s,0);

	if (INDEX(_sig,"TSNE") ge 0) {
	  _u = "/cm^3";
	  _array = _array * 1.e-6;
	} else {
	  _u = UNITS_OF(_s);
	}

	_zval = ADJUSTL(CVT(_z[_index],"123456789."));
	_units = _u //" at Z = "// EXTRACT(0,INDEX(_zval,"S"),_zval) // " cm";

	/*** IF DO THE FOLLOWING, THERE IS NO SEG FAULT IN MDSGETDATTEST:
	     _units = "blah";
	***/

        _sig = MAKE_SIGNAL(MAKE_WITH_UNITS(_array,_units),,_d); 

	return(_sig);

}
		
	
