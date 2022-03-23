#include "PythonConstructor.h"

#include <iostream>

namespace facesynthesizing::construction {
	PythonConstructor::PythonConstructor()
	{
		faceSynthesizer = std::make_shared<python::FaceSynthesizer>();

        this->initializePython();
	}
    void PythonConstructor::resolveDependencies()
    {
        faceSynthesizer->setGlobals(globals);
    }
    std::shared_ptr<python::FaceSynthesizer> PythonConstructor::getFaceSynthesizer()
    {
        return faceSynthesizer;
    }
    void PythonConstructor::addHigherLevelDependency(std::shared_ptr<usecases::FaceSynthesizingProjectInteractor> faceSynthesizingProjectInteractor)
    {
        faceSynthesizer->setFaceSynthesizingProjectInteractor(faceSynthesizingProjectInteractor);
    }
	void PythonConstructor::initializePython()
	{
        python::addPythonModules();

		Py_Initialize();
        np::initialize();

        try {
            py::object main = py::import("__main__");
            globals = main.attr("__dict__");

            addVenvSitePackages();
            setCurrentWorkingDirectory();
        }
        catch (py::error_already_set const&)
        {
            PyErr_Print();
            PyErr_Clear();
        }

        std::cout << "Initialized Python" << std::endl;
	}
    void PythonConstructor::addVenvSitePackages()
    {
        py::exec(
            "import os\n"
            "import sys\n"
            "venv_site_packages = os.path.join(os.getcwd(), '.venv', 'Lib', 'site-packages')\n"
            "if not os.path.exists(venv_site_packages):\n"
            "    raise RuntimeError('Could not find virtual environment site packages at location: {}."
            "format(venv_site_packages)')\n"
            "sys.path.insert(0, venv_site_packages)\n"
            "print('Python venv site-packages: {}'.format(venv_site_packages))\n"
            , globals
        );
    }
    void PythonConstructor::setCurrentWorkingDirectory()
    {
        py::exec(
            "current_working_directory = os.path.join(os.getcwd(), 'face-synthesizer-src')\n"
            "os.chdir(current_working_directory)\n"
            "sys.path.insert(0, os.getcwd())\n"
            "print('Python Current Working Directory: {}'.format(os.getcwd()))\n"
            "sys.argv = ['']\n"
            , globals
        );
    }
}