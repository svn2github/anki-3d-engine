#include "ScriptCommon.h"
#include "r/RenderingPass.h"


WRAP(SwitchableRenderingPass)
{
	class_<SwitchableRenderingPass, noncopyable>("SwitchableRenderingPass",
		no_init)
		.def("getEnabled", (bool (SwitchableRenderingPass::*)() const)(
			&SwitchableRenderingPass::getEnabled))
		.def("setEnabled", &SwitchableRenderingPass::setEnabled)
	;
}
