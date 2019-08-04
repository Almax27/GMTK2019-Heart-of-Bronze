// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class GMTK_Jam_2019EditorTarget : TargetRules
{
	public GMTK_Jam_2019EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "GMTK_Jam_2019" } );
	}
}
