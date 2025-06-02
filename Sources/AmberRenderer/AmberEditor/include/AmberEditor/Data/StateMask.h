#pragma once

namespace AmberEditor::Data
{
	//TODO: real mask
	struct StateMask
	{
		bool DepthWriting;
		bool DepthTest;
		bool ColorWriting;
		bool Blendable;
		bool BackFaceCulling;
		bool FrontFaceCulling;
	};
}
