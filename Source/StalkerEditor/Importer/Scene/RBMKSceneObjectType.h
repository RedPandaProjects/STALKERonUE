#pragma once

enum class ERBMKSceneObjectType: uint32
{
    AllTypes            = 0xFFFFFFFF,
    Group       		= 0,
    Glow        	   	= 1,
    StaticMesh          = 2,
    Light       	   	= 3,
    Shape             	= 4,
    SoundSource      	= 5,
    SpawnPoint      	= 6,
    Way         	  	= 7,
    Sector          	= 8,
    Portal          	= 9,
    SoundEnvironment 	= 10,
    ParticleSystem	   	= 11,
    DetailObject		= 12,
    AIMap		        = 13,
    Wallmark			= 14,
    FogVolume	        = 15,
    Count		        = 16,
	Unkown,
};