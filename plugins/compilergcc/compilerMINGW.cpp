#include "compilerMINGW.h"
#include <wx/intl.h>

CompilerMINGW::CompilerMINGW()
    : Compiler(_("MinGW Compiler Suite"))
{
#ifdef __WXMSW__
	m_MasterPath = "C:\\MinGW";
	
	m_Programs.C = "mingw32-gcc.exe";
	m_Programs.CPP = "mingw32-g++.exe";
	m_Programs.LD = "mingw32-g++.exe";
	m_Programs.WINDRES = "windres.exe";
	m_Programs.MAKE = "mingw32-make.exe";
#else
	m_MasterPath = "/usr";
	
	m_Programs.C = "gcc";
	m_Programs.CPP = "g++";
	m_Programs.LD = "g++";
	m_Programs.WINDRES = "";
	m_Programs.MAKE = "make";
#endif
	m_Switches.includeDirs = "-I";
	m_Switches.libDirs = "-L";
	m_Switches.linkLibs = "-l";
	m_Switches.defines = "-D";
	m_Switches.genericSwitch = "-";
	m_Switches.objectExtension = "o";
	m_Switches.needDependencies = true;
	
	m_Options.AddOption(_("Produce debugging symbols"),
				"-g",
				_("Debugging"), 
				"",
				true, 
				"-O -O1 -O2 -O3 -Os", 
				_("You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."));
	m_Options.AddOption(_("Profile code when executed"), "-pg", _("Profiling"), "-pg -lgmon");
	m_Options.AddOption(_("Enable all compiler warnings"), "-Wall", _("Warnings"));
	m_Options.AddOption(_("Optimize generated code (for speed)"), "-O", _("Optimization"));
	m_Options.AddOption(_("Optimize more (for speed)"), "-O1", _("Optimization"));
	m_Options.AddOption(_("Optimize even more (for speed)"), "-O2", _("Optimization"));
	m_Options.AddOption(_("Optimize generated code (for size)"), "-Os", _("Optimization"));
	m_Options.AddOption(_("Expensive optimizations"), "-fexpensive-optimizations", _("Optimization"));

    m_Commands[(int)ctCompileObjectCmd] = "$compiler $options $includes -c $file -o $object";
    m_Commands[(int)ctGenDependenciesCmd] = "$compiler -MM $options -MF $dep_object -MT $object $includes $file";
    m_Commands[(int)ctCompileResourceCmd] = "$rescomp -i $file -J rc -o $resource_output -O coff $res_includes";
    m_Commands[(int)ctLinkExeCmd] = "$linker $libdirs -o $exe_output $libs $link_objects $link_options";
    m_Commands[(int)ctLinkDynamicCmd] = "$linker -shared -Wl,--output-def=$def_output -Wl,--out-implib=$static_output -Wl,--dll $libdirs $link_objects $libs -o $exe_output $link_options";
    m_Commands[(int)ctLinkStaticCmd] = "ar -r $exe_output $link_objects\n\tranlib $exe_output";
}

CompilerMINGW::~CompilerMINGW()
{
	//dtor
}

Compiler * CompilerMINGW::CreateCopy()
{
    return new CompilerMINGW(*this);
}
