-----
INTRODUCTION
-----
 
      Hello! Thank you for purchasing Ultra Dynamic Sky! 
 
      This document will cover all the basics of setting up and using UDS, but if you have any questions or run into any problems not covered here, please email me at support@ultradynamicsky.com 
 
      Also, there are tooltips for just about every setting on the sky and weather, so if you're confused about what something does, hover over it for a specific explanation. 
 
-----
SKY SETUP
-----
 
-----
ADDING ULTRA DYNAMIC SKY TO YOUR LEVEL
-----
 
      First, make sure you've removed any existing sky and cleared the following out of your level: 
 
      - Directional Lights 
 
      - Sky Light 
 
      - Exponential Height Fog 
 
      - Sky Atmosphere 
 
      - Volumetric Cloud 
 
      With all of these removed, if there is still some ambient light present, that probably means there's still static light left over in the level's lightmaps, so run a lighting build (from the Build dropdown on the main toolbar) to clear that out. 
 
      Drag the Ultra Dynamic Sky actor into your scene from the Blueprints folder. You should see the sky with its default configuration of daytime lighting with volumetric clouds. 
 
      Typically you should place UDS at ground level. If you want to change the height of the clouds, use Bottom Altitude in the Volumetric Clouds category instead of moving the actor. 
 
      Note, depending on the context, the editor may need to compile shaders for UDS before all of its features become visible. The editor should indicate this is happening, and it can potentially take a while depending on your system specs. 
 
-----
BASIC CONTROLS
-----
 
      When you select UDS in your level, the details panel will show the UDS settings, starting with the Basic Controls category. 
 
      The main things to note in Basic Controls are: 
 
      - Time of Day is the main control for time on UDS, and by default will determine the position of the sun and the moon. 
 
      - Sky Mode, Color Mode, and Project Mode, which broadly determine the features and behavior of UDS. See the sections below for information about each. 
 
      - Cloud Coverage and Fog, which determine how cloudy the sky is and how foggy the scene will be. Note, if Ultra Dynamic Weather also exists in your level, these will be controlled by UDW's weather state. 
 
      - Simple adjustments like Saturation, Contrast, Overall Intensity, and Night Brightness. These can be used for basic visual tweaks of the sky and lighting. 
 
      - Dusk Time and Dawn Time determine the Time of Day values where the sun will cross the horizon. Note, these values won't be used if the sun is using Simulate Real Sun. For more information about the Simulation features, see the section on Simulation. 
 
-----
SKY MODE
-----
 
      In Basic Controls on UDS, there is a setting called Sky Mode. This mainly controls the type of clouds (or volumetric aurora) in use. Here is a quick explanation of each option: 
 
      1. Volumetric Clouds - Enabled by default, this sky mode uses a field of full 3D clouds. This can offer the most realistic visuals, but at a performance cost that may be too much for some projects/platforms. 
 
      2. Static Clouds - Designed to mimic the look of the volumetric clouds but as a static cloud texture. Much lower performance cost than volumetric clouds. 
 
      3. 2D Dynamic Clouds - Dynamic clouds drawn using panning 2D textures. 
 
      4. No Clouds - This mode uses no dynamic clouds at all. 
 
      5. Volumetric Aurora - Sacrifices clouds to render full 3D aurora effects. Adjustable using the settings in the Aurora category. 
 
      6. Space - For scenes set in outer space, with no planet/ground beneath the viewer. Disables all clouds, atmosphere, sky coloring, leaving only sun, moon and stars. 
 
-----
COLOR MODE
-----
 
      The next setting in Basic Controls, called Color Mode, determines where the colors for the sky come from. 
 
      1. Sky Atmosphere - The system will use unreal's sky atmosphere rendering system to imitate correct atmospheric scattering. The settings for this can be further adjusted from the Sky Atmosphere category. 
 
      2. Simplified Color - Determines colors in simpler ways, by sampling an atmospheric scattering LUT texture, along with adjustable color values. Cheaper on the material level than the sky atmosphere, and more directly customizable. The color values used by Simplified Color can be changed from the Simplified Color category. 
 
-----
PROJECT MODE
-----
 
      The third top level Mode setting in Basic Controls is Project Mode. This setting determines how the system will behave at runtime. There are two options for Project Mode: 
 
      1. Game / Real-time enables runtime optimizations which spread out calculations on UDS. It's intended for games or any kind of project where real-time performance is needed. 
 
      2. Cinematic / Offline disables these optimizations, so that everything dynamic on the sky will update fully every frame. It also increases many quality settings automatically at runtime, to maximize render quality. It's intended for rendering movies or stills, or any project where real-time performance is not needed. 
 
-----
CLOUDS
-----
 
-----
VOLUMETRIC CLOUDS
-----
 
      The Volumetric Clouds category is where you'll find the settings for adjusting the appearance of the volumetric cloud layer. Here are some of the key settings and what they do: 
 
      - Volumetric Cloud Rendering Mode is what determines how the engine will render the volumetric clouds to the screen. It has performance implications, and determines if clouds can render in front of objects or just in the background layer. See the tooltips for each option for an explanation of each. 
 
      - Bottom Altitude is what sets how high the cloud layer is in the level. It's recommended to use this option instead of moving the UDS actor, as Bottom Altitude will adjust only the cloud layer, and won't have unintended effects on atmosphere and fog. 
 
      - Volumetric Clouds Scale is what you should adjust if you want to scale the entire cloud field uniformly. 
 
      - The View Sample Scale settings are what control the amount of samples used to render the clouds. If they're looking too noisy, you can increase this setting to resolve the clouds better, at a performance cost. 
 
      - Two Layers can be enabled to add a second layer of clouds above the first, offset in a handful of ways using the Layer 2 settings below that. Note that a second layer will increase the cost of cloud rendering very significantly. 
 
      If you want to directly control volumetric cloud coverage in specific parts of your level, like to make a specific area around a mountain free of clouds for example, you should use the Volumetric Cloud Painter utility. There is a section specifically on the Volumetric Cloud Painter further down in this document. 
 
-----
2D DYNAMIC CLOUDS
-----
 
      In the 2D Dynamic Clouds category, you'll find all the settings relating to the look of the 2D clouds, from softness and shading effects to the noise texture used to generate the clouds. 
 
      When adjusting the 2D Dynamic Clouds, understand that the clouds are a material effect which exists on the sky sphere, so they do not exist in the 3D space of the level. Settings like Cloud Height are just a control of the fake perspective distortion that exists in the sky material. 
 
      Also note, in the advanced dropdown of the 2D Dynamic Clouds category, you'll find settings to adjust the distribution of the clouds, to make them have more coverage around the horizon for example, or reduce coverage around the moon. 
 
-----
STATIC CLOUDS
-----
 
      The Static Clouds category has a few settings for changing out the texture used by the static clouds, and controlling its rotation. 
 
      There are three static clouds textures included, but you can also create your own custom ones using a special editor utility. The Static Clouds Authoring Tool, found in Blueprints/Tools, can render the volumetric clouds on UDS to create a new static clouds texture. 
 
-----
CLOUD MOVEMENT
-----
 
      The Cloud Movement category contains the settings for how volumetric clouds and 2D dynamic clouds will move. You can adjust Cloud Speed and Cloud Direction from here. Note that Cloud Direction will be controlled entirely by UDW if UDW exists in the level, using its wind direction. 
 
      Cloud Phase is particularly useful when Cloud Speed is set to 0. When clouds are still, you can use Cloud Phase to fine tune an exact formation of clouds you like. 
 
      Randomize Cloud Formation On Run will start the game with a randomized cloud formation every time, independent of the other movement settings. 
 
      Clouds Move With Time of Day is useful for situations where the speed of time can change, to keep cloud movement in sync with time during a short time lapse for example. 
 
-----
CLOUD WISPS
-----
 
      Cloud Wisps are the layer of static, wispy high altitude clouds, which are rendered on the sky behind every sky mode (except Space). You can find the settings for adjusting the appearance of them in the Cloud Wisps category. 
 
      The cloud wisps work by sampling a grayscale texture and using that to apply a cloud wisp color, which changes with time of day. The opacity of that color is scaled using the Cloud Wisps Opacity settings, one of which is used when Cloud Coverage is low, and one is used when coverage is high. 
 
      The texture can be replaced with a custom one with the Cloud Wisps Texture setting. Again, this should be a grayscale texture, as only the R channel will be sampled, to apply the cloud wisps color to the sky. 
 
-----
LIGHTING
-----
 
-----
LIGHT COMPONENTS
-----
 
      UDS has light components for the directional lights and sky light. The components (or actors) used will be determined by the settings in the Component Selection category, and in the case of the sky light, the Sky Light Mode setting. 
 
      Note that while it's possible to have UDS reference a light actor for the sun, moon or sky light, instead of using the built in component, this is not recommended in any situation except those which specifically require a light actor. For example third party assets or plugins may occasionally require a light actor reference. This is really the only situation it should be necessary to use an actor instead of the built in components. 
 
      If you do have to use light actors in place of the components, note these additional steps: 
 
      - For the directional lights, there is a setting on the component called "Cloud Scattered Luminance Scale" which controls how the lights affect the clouds. This isn't something exposed to blueprint control, so UDS cannot apply it to your light actor itself. You'll need to copy the value for Clouds Scattered Luminance Scale from the Sun or Moon component on UDS to your directional light actor. 
 
      - Similarly, on a sky light there is a setting called Source Type which UDS cannot control, but needs to be set in accordance with UDS' Sky Light Mode setting. For "Cubemap with Dynamic Color Tinting" and "Custom Cubemap", make sure the Source Type on your sky light actor is Specified Cubemap. For the sky light mode called Capture Based, make sure the source type is set to Captured Scene. 
 
-----
SUN AND MOON DIRECTIONAL LIGHTS
-----
 
      The directional light components on UDS are called Sun and Moon. Several settings on the directional lights will be controlled by the blueprint, such as light color, intensity, cast shadows, and light source angle. But the rest will be left open for you to edit directly on the component. To do this, just select UDS and find the Sun or Moon in the component list at the top of the details panel. On selecting one, you will see its directional light component settings, the same as the ones on a directional light actor. 
 
      The exposed settings for UDS' control of the directional lights can be found in their respective categories in the UDS details: Sun and Moon. 
 
      For information about the positioning and movement of the sun and moon, and their appearance in the sky, see the Sun and Moon section below. 
 
-----
CLOUD SHADOWS
-----
 
      Cloud Shadows will be applied to the directional lights by default, and are compatible with all sky modes, including the ones without clouds. They can be disabled from the Cloud Shadows category. 
 
      The Cloud Shadows category is also where you can adjust the intensity and softness of the shadows with cloud coverage. By default, cloud shadows fully occlude the directional light in high coverage, but only partially in low coverage. From here you can also adjust how much directional light is shadowed when Fog or Dust Amount is high. Note that this is a part of the cloud shadows light function, so it also requires Use Cloud Shadows to be enabled. 
 
      If the sky mode is volumetric clouds, then the cloud shadows will be accurate to the 3D layer, so that every cloud in the layer casts its own correct shadow on the scene. For the other sky modes, since they don't have clouds which exist in 3D space to cast direct shadows, the speed and scale of the shadows texture is determined by 2D Cloud Shadows Speed and 2D Cloud Shadows Scale. 
 
-----
SKY LIGHT
-----
 
      In the Sky Light category, Sky Light Mode determines the source of the sky light cubemap and how its intensity and color will be controlled by UDS. 
 
      These are the Sky Light Modes and how they work: 
 
      1. Capture Based - The sky light will generate a cubemap by capturing the sky. By default, this method uses the real time capture option. If real time capture isn't necessary for your project, you can disable the real time capture from the Capture Based Sky Light category. Note that Real Time Capture is not supported by all platforms, and is incompatible with Volumetric Fog without Lumen. 
 
      2. Custom Cubemap - A simple static cubemap texture, set using the Custom Cubemap option below. 
 
      3. Cubemap with Dynamic Color Tinting - This method uses a flat gray cubemap and tints it dynamically with the fog color to align well with the time of day. This option is good for performance in projects with dynamic time of day, as the ambient light is dynamic without requiring recapturing, and supports all platforms. 
 
      Each Sky Light Mode has its own category of settings specific to the mode, found beneath the main Sky Light category. 
 
      In the main Sky Light category itself, you'll find settings which apply to all 3 modes, like the Sky Light Intensity. You can also use the Sky Light Color Multiplier settings to tint the color differently based on Time of Day. Note that these color multipliers can also be set to values higher than 1 and this will scale the light intensity proportionally at that time. 
 
-----
EXPOSURE
-----
 
      By default, UDS applies auto exposure settings. Auto exposure is what allows the brightness of the image to adapt to the light level of the scene automatically. Some features in unreal occasionally refer to auto exposure as "eye adaptation". 
 
      These auto exposure settings can be adjusted, or disabled, from the Exposure category. 
 
      When Apply Exposure Settings is enabled, UDS will apply the auto exposure mode "Auto Exposure Histogram", and control these post process settings for exposure: 
 
      - Exposure Componsation Curve, which pushes image brightness higher or lower based on the general level of light in the image. 
 
      - Exposure Compensation, controlled on UDS by the Exposure Bias settings for different time and weather scenarios. This can allow you to specifically make nighttime exposure brighter, or cloudy exposure darker, for example. 
 
      - MinEV100 and MaxEV100, which control the range of brightness that auto exposure will try to adapt to. This is controlled on UDS with Exposure Min Brightness and Exposure Max Brightness. 
 
      If you're trying to fine tune auto exposure behavior with these settings and/or the additional exposure settings in a post process volume or camera, I'd recommend using the viewport view mode found in Show > Visualize > HDR(Eye Adaptation) to see exactly what auto exposure is doing behind the scenes. 
 
-----
USING STATIC/STATIONARY LIGHTING
-----
 
      UDS' default configuration uses all movable lights, but it supports static lighting as well. 
 
      To make any of UDS' lights static or stationary, find its mobility setting, in the UDS details. These are found in the lights' respective categories, so Sun, Moon, and Sky Light. Make sure you set the mobility from the exposed setting, and not on the light component itself. 
 
      Once you've built the level's lighting, you will still be able to make changes to the sky settings, but note that any settings which alter the lights or move them around will break the lighting build and you'll have to build again. Also note that if your sun light is set to be static/stationary, this disables the Animate Time of Day setting, as the sun can no longer be moved in game. 
 
-----
TIME
-----
 
-----
SETTING UP A DAY/NIGHT CYCLE
-----
 
      If you want Time of Day to move forward on its own at runtime, to create a day/night cycle, the place to set that up is the Animate Time of Day category. 
 
      If Animate Time of Day is enabled there, time will move forward when the level is running. The speed of time will be determined by Day Length and Night Length. These values are in minutes and represent the real time that will pass between sunrise and sunset and vice versa. 
 
      Note, if you have Simulate Real Sun enabled in the Simulation category, the speed time moves is determined by Simulation Speed there, not Day Length and Night Length. For more info about the simulation features, see the Simulation section in these docs. 
 
-----
CONTROLLING TIME OF DAY
-----
 
      Change the Time of Day property in Basic Controls to adjust the time of day the system will start with. You can also enable an option to start with a random time, from the Animate Time of Day category. 
 
      For controlling Time of Day at runtime, there are several ways you can do that: 
 
      - Simply setting Time of Day on UDS at runtime will instantly change the time directly. 
 
      - You can call the functions on UDS called "Set Time of Day Using Time Code" or "Set Date and Time" to use common time/date structures to set the time with hours/minutes values. 
 
      - You can call the function "Transition Time of Day" on UDS, to have the time start animating to a target value over a specified duration. This is useful if you want to have time pass quickly to a particular point. 
 
-----
GETTING AND DISPLAYING TIME OF DAY
-----
 
      To get Time of Day from UDS, there are functions which can output the time in different formats: 
 
      - "Get Time of Day in Real Time Format" will output a Time Code structure, with hours/minutes/seconds values. 
 
      - "Get Current Date and Time" will output a Date Time structure, with the time in hours/minutes/seconds as well as the simulation date. 
 
      - "Is It Daytime?" will output a boolean which is true if the sun is up. If rather than the specific time you just need to know if it's day or not, this function is the one to use. 
 
      For displaying the time on screen in a simple way, UDS comes with two clock widgets. These can be found in the widget designer palette in the Ultra Dynamic Sky Widgets category. 
 
-----
SUN AND MOON
-----
 
-----
ADJUSTING SUN AND MOON MOVEMENT
-----
 
      By default (without using the Simulation settings covered elsewhere in this document) the Sun and Moon's position in the sky with time will be derived using the following settings found in both the Sun and Moon categories: 
 
      - Angle (Sun Angle and Moon Angle) - This controls the yaw of the sun or moon's path. In other words, the direction that it will set/rise. It's a value in degrees, rotating the path clockwise. 
 
      - Inclination (Sun Inclination and Moon Inclination) - This pitches the path, so that the sun or moon's highest position in the sky will be lower, angled away from the zenith. It's also a value in degrees, with 0 meaning the sun/moon would go all the way to the top of the sky at midday. 
 
      - Vertical Offset (Sun Vertical Offset and Moon Vertical Offset) - This value can be positive or negative, to push the position of the sun or moon higher or lower, irrespective of the time. 
 
      - The moon specifically also has a setting for Moon Orbit Offset which controls how the moon is offset from the sun in time. A value of 0 will have the moon's path be the opposite of the sun's, rising at night and going down during the day. 
 
      Again, it's important to note all of these values will be overridden by the Simulation settings if enabled. See the section in these docs on the Simulation features for more information on how to control that. 
 
-----
MANUALLY POSITIONING THE SUN AND MOON
-----
 
      It is possible to disconnect the sun or moon's position from time, and place them completely manually. 
 
      To do this, check Manually Position Sun Target in the Sun category, or Manually Position Moon Target, in the Moon category. 
 
      With that turned on, find the blue diamond shaped widget labeled Sun Target or Moon Target, visible on the UDS actor when it's selected. Select that widget and move it to adjust the position of the sun or moon. 
 
      The direction of the sun/moon will be determined by the direction from the UDS actor to the position of that target widget. 
 
-----
SUN AND MOON APPEARANCE
-----
 
      The Sun and Moon categories are where you'll find settings for the appearance of the sun and the moon as they are rendered in the sky material. 
 
      Sun 
 
      The sun is rendered as a simple radial gradient, so it just has settings for radius, softness, color, and intensity. 
 
      Sun radius will also be used to derive the correct light source angle of the sun directional light, so the softness of the shadows and size of specular highlights will be correct for the sun's size in the sky. 
 
      Moon 
 
      Since the moon is rendered using a texture, it has more settings for its appearance. 
 
      The default moon texture is built to support rendering moon phases, which can be adjusted with the Moon Phase setting. You can also enable an option in the Moon category to have moon phase change over time. Note that if using Simulate Real Moon, the moon phase will be set by the simulation. 
 
      You can select a texture to replace the default moon texture with Custom Moon Texture, but note that this will remove the moon phase functionality. If you want to try authoring a texture which supports phases the same way the default one does, the setting for that is the Moon Masks Texture, in the advanced dropdown of the Moon category. 
 
-----
FOG
-----
 
-----
FOG DENSITY
-----
 
      The Fog Density category on UDS is where variables like Cloud Coverage and Fog are used to determine the density, height falloff, and start distance of the exponential height fog layer. 
 
      Density is how thick/opaque the fog will be with distance from the camera. Height Falloff is how quickly fog density disppates above the height of the fog component. 
 
      For density, there is a base value, and then values added to density for Cloud Coverage, Fog, and Dust Amount. 
 
      For height falloff, there is a base value, and then falloff values for a cloudy, foggy, and dusty state, which are used directly, not added together. 
 
      The start distance of the fog is derived from the density, using a value for start distance at low density, and a density value where start distance will be reduced to 0, to allow fog right next to the camera. 
 
-----
FOG COLOR
-----
 
      The Fog Color category is where the settings for the color of the height fog are, but how they're used by UDS depends heavily on the project setting "Support Sky Atmosphere Affecting Height Fog". 
 
      This setting should typically be enabled by default in new projects, and I would recommend leaving it enabled, as it derives colors for fog directly from the sky atmosphere, and this usually produces good, desired results. If this project setting is on, only some of the Fog Color settings on UDS will be used by the fog. Specifically the settings which scale intensity and brightness, but not the settings for direct control of colors/saturation. 
 
      If you want more direct control over fog color on its own however, you might try disabling that project setting, to allow the fog to always use the Fog Color settings alone to determine fog colors. 
 
      Note, you can disregard everything above if you are using the Simplified Color option for Color Mode, as the sky atmosphere is not enabled at all in that mode. If Simplified Color is selected, all the Fog Color settings will be used by the fog. 
 
-----
VOLUMETRIC FOG
-----
 
      Volumetric Fog is a rendering feature in Unreal Engine which can render more advanced fog nearby the camera, which recieves and scatters light from local light sources and can vary its color and extinction using volumetric materials or particles placed in the level. 
 
      The settings for enabling this on UDS are found in its Volumetric Fog category. 
 
      The basic settings for enabling volumetric fog and adjusting its overall extinction and the distance it renders from the camera are found at the top of the category. You'll also find settings for the sun and moon's Volumetric Scattering Intensity here, which scales how much each light source lights up the volumetric fog. 
 
      Below that are the options for the Global Volumetric Material, which if enabled will allow some simple global controls of extinction and color. The global volumetric material has the following features, each of which can be enabled/adjusted from here in the Volumetric Fog category: 
 
      - Texture based 3D noise applied to the fog extinction, to add some variation to the fog. 
 
      - The ability to mask the volumetric fog using the Weather Mask brush system. See the weather section of this readme for information about that toolset. 
 
      - Ground fog, which adds additional fog extinction in the air directly above distance fields. This requires the project setting to generate mesh distance fields to be enabled. 
 
      - Specific fog extinction and albedo can be applied beneath the water level. The water level is controlled from the Water category. 
 
      When volumetric fog and volumetric clouds are used together, they can sometimes produce strange ghosting artifacts in the areas they overlap. This will be influenced some by the Volumetric Cloud Rendering Mode in the Volumetric Cloud category. The one I'd recommend for best results alongside Volumetric Fog is "Clouds Render Over Opaque Objects (Quality)". 
 
-----
SKY COLOR
-----
 
-----
SKY ATMOSPHERE
-----
 
      If Color Mode is set to Sky Atmosphere, then the main controls of sky color will be found in the Sky Atmosphere category. Here are some of they key settings: 
 
      - The Rayleigh Scattering settings are what chiefly determine the main color of the sky. It can be adjusted for day, night, and dawn/dusk individually. 
 
      - The Sunset/Sunrise and Twilight values control the Absorption setting on the sky atmosphere component. This can be used to make stronger colored sunsets for example, or alter the tone of twilight. 
 
      - Sky Atmosphere Overcast Luminance will scale the intensity of all sky colors whenever the Cloud Coverage is high. 
 
-----
SIMPLIFIED COLOR
-----
 
      If Color Mode is set to Simplified Color, then the main controls of sky color will be found in the Simplified Color category. Here are some of they key settings: 
 
      - The color variables in Simplified Color control the base sky color, as well as the light and dark color of the 2D Dynamic Clouds and Static Clouds. (For control of volumetric cloud color, see the Volumetric Cloud category.) 
 
      - The Directional Intensity float curve controls how much the intensity of the directional light is dimmed down as it nears the horizon. 
 
      - The Directional Light Scattering color curve controls how the directional light color is filtered as it nears the horizon. 
 
      - The Atmospheric Scattering LUT texture is a special HDR lookup table which is used to imitate the appearance of atmospheric scattering for the sun and the moon. The Y axis represents the height of the sun or moon relative to the horizon, and the X axis is roughly the height of the specific pixel being rendered for that point in the sky. It is very specifically and carefully authored, and if you want to customize it, I would suggest exporting it and making modifications rather than trying to make your own from scratch. 
 
-----
SIMULATION
-----
 
-----
SIMULATION OF REAL WORLD SUN, MOON AND STARS
-----
 
      UDS has features to position the sun, moon and stars using real world coordinates and date. These options are found in the Simulation category. 
 
      To enable them, just check Simulate Real Sun, Simulate Real Moon, and Simulate Real Stars. Each of these can also be enabled individually as needed. 
 
      - Simulate Real Sun and Simulate Real Moon simply override the default logic for the positioning of the sun and moon. All of the settings for positioning/path in the Sun and Moon categories will be overridden (except for Manually Position, which takes priority over the Simulation) 
 
      - Simulate Real Stars will change what texture is used for the stars. by default a simple tiling stars texture is used. With Simulate Real Stars enabled, a 360 degree star map will be used instead. Both of these texture settings are exposed to be changed out in the Stars category. 
 
      The simulated location is determined using the Latitude and Longitude values. Also make sure to set the Time Zone to the correct UTC offset for the location, so that Time of Day will be interpreted correctly as local to this location. You can also select a city preset, which will apply latitude, longitude and a time zone value in one step. 
 
      The year, month and day control the date of the simulation. At runtime, the date will increment one day forward every night at midnight, like you would expect. 
 
      North Yaw can be adjusted to change what world space direction corresponds to north in the simulation. It's a value in degrees, rotating the simulation clockwise. By default, north is toward positive X. 
 
      Notes on the Simulation Accuracy 
 
      Because the complete versions of the appropritate astronomical algorithms would be very expensive to calculate with blueprint math, what I've created here is my own approximation. It leaves out complicating nuances such as the eccentricity of the earth's orbit, or the irregularity of the earth's shape. That said, I've done a lot of back and forth testing, and my approximations of the sun's position were never more than a little over a degree off from the results given by the complete algorithm, and that deviation is a worst case scenario. Typically the deviation was only in fractions of 
a degree. Again, based on my own tests. You may find edge cases where the deviation is worse. Still, I think the accuracy of the sun is sufficient for almost all practical applications. 
 
      The accuracy of the Moon's position is more variable than the sun's. The Moon's orbit is calibrated using it's position in 2017. So, any date within several decades of that should be generally accurate. At the very least, the moon should generally appear in the correct constellation and have the right phase. 
 
-----
CONTROLLING TIME OF THE SIMULATION
-----
 
      If Simulate Real Sun is enabled, Time of Day will behave differently in some ways to suit the needs of the simulation. Also the simulation has some specific features which interpret or change how time is handled. 
 
      - If Animate Time of Day is enabled, the speed of time will not be determined by Day Length and Night Length. Instead, it will be determined by Simulation Speed, in the Simulation category. This value scales the speed of time, with 1 being roughly realistic speed. 
 
      - Options for daylight savings time can be found in the Simulation category. If enabled, Time of Day will be interpreted with DST within the range of the year set by the DST start and end times in the advanced dropdown of the Simulation category. 
 
      - Use System Time, in the Simulation category, will disconnect the simulation from Time of Day and time the simulation using the system UTC time. This means that if the system clock is correct, the simulation will be accurate to the current real world location right now. Note, because this option gets the system's UTC time directly, there is no need to convert anything from a local time zone, so the Time Zone setting is not used. 
 
-----
CINEMATICS AND RENDERING
-----
 
-----
ANIMATING UDS WITH SEQUENCER
-----
 
      Here are some tips for animating the sky using sequencer. 
 
      Keyframing Time of Day 
 
       To animate Time of Day directly, you should disable the Animate Time of Day setting on UDS, and keyframe the Time of Day variable. When animating with the Time of Day variable, it's okay to go past 2400. The system will understand this and proceed forward in time with a new day. 
 
      Keyframing Cloud Coverage or Fog 
 
       To animate the cloudiness or fogginess, you can keyframe the Cloud Coverage or Fog variables on UDS, but only if you don't have an Ultra Dynamic Weather actor in your scene. If you do, you should keyframe the Cloud Coverage and Fog variables on UDW instead, using its manual weather state. (See the sequencer section in Weather for more info about animating weather state on UDW) 
 
      Keyframing Cloud Movement 
 
       To directly animate cloud movement, first set Cloud Speed to 0. Disable "Randomize Cloud Formation on Run" and "Clouds Move with Time of Day" as well. Then in sequencer, animate using the Cloud Phase variable. This will allow you to directly control cloud movement in a way that will play back the same way every time. 
 
      Keyframing Other Variables 
 
      Not every variable on UDS is reapplied dynamically at runtime, in the way that would be needed to animate the value in a cinematic. Many are though, and all of the UDS variables that will work without additional steps are exposed to be keyframed. 
 
      If you want to try keyframing additional variables that are not exposed by default, you'll need to mark them as exposed. Open UDS in the blueprint editor, find the variable in the My Blueprint panel, and check Expose to Cinematics on its variable details. 
 
      Any variables you expose manually like that will probably not update with the keyframe at runtime, but you could try calling the Update Static Variables function every frame of the animation, and that should work to force most of the remaining values to update. 
 
      For tips about animating weather, see the section about sequencer in the Weather section of the readme. 
 
-----
RENDERING MOVIES
-----
 
      When rendering out a sequence using the movie render queue, first make sure to change Project Mode on UDS to "Cinematic / Offline". This configures the sky for maximum quality at runtime, while also disabling many optimizations that are useful only in real-time rendering scenarios. If rendering a movie using the path tracer, see the section in this document on using UDS with the path tracer. 
 
      If your project does need real-time performance at runtime normally , be sure to turn Project Mode back to Game / Real-time after completing your movie render, or game performance will be severely compromised. 
 
-----
RENDERING WITH THE PATH TRACER
-----
 
      If you turn on the Path Tracer in a scene with Ultra Dynamic Sky, you'll face some rendering limitations of the path tracer. Specifically it cannot yet natively render some of the sky rendering features (like volumetric clouds) which UDS makes use of. The path tracer will instead use the sky light cube map as a replacement for native sky rendering. 
 
      Ultra Dynamic Sky has a setting specifically to adjust for these limitations as well as it can, to work accepably as a background for path traced renders. Just enable the option "Adjust for Path Tracer" in the advanced dropdown of the Basic Controls category on UDS. 
 
-----
CONTROLLING YOUR LEVEL WITH UDS
-----
 
-----
TURNING ON LIGHTS IN YOUR LEVEL AT NIGHT
-----
 
      There is a special component included to help make a light which turns on and off at sunset/sunrise. It's the "Light Day-Night Toggle" component found in the Blueprints/Utilities folder. 
 
      Just add one of these components as a child of any light component on an actor. When the level is played, the light's active state will be determined by if the sun is up or not. It will be on at night by default, but you can change that using a setting on the component. 
 
      By default the light will also animate its intensity as it powers on, to imitate a flourescent lamp. You can disable this or adjust it from the component's settings. 
 
      The state of the light can also be used to control a "Light Toggle" parameter on an array of dynamic material instances. Just add a dynamic material instance to that array, and the parameter (from 0 to 1) will be set as the state of the light changes. 
 
-----
TRIGGERING EVENTS AT SPECIFIC TIMES
-----
 
      UDS has event dispatchers which fire at particular times, which your actors can use to perform actions at Sunset, Sunrise, Midnight, or Hourly. 
 
      To use them, you'll first need to get a reference to the UDS actor in your blueprints. A simple way to do that is to use "Get Actor of Class". Then using that reference, you can bind events in your blueprint to the dispatchers. For example "Bind to Sunrise" or "Bind to Sunset". Once they're bound, these events will be called by UDS at the appropriate time. 
 
      By default, the Sunset and Sunrise events will fire at the exact moment the sun crosses the horizon. But you can offset that time to suit your needs using the "Sunrise Event Offset" and "Sunset Event Offset" settings in the Animate Time of Day category. 
 
-----
ADDITIONAL FEATURES
-----
 
-----
ADJUSTING SETTINGS INSIDE INTERIORS
-----
 
      UDS has a category called Interior Adjustments, which if enabled, you can use to adjust things like fog and light, based on how occluded the player camera is at runtime. 
 
      To use these settings, first check "Apply Interior Adjustments" in the Interior Adjustments category. Then, make your changes to the settings below, which modify various properties of the sky when inside an interior. 
 
      Note, the interior adjustments can only test and respond to the player camera's location at runtime, so you'll need to play the level to see the effects. It's not something you'll see moving the editor camera around the level. 
 
      The settings for the occlusion system are stored in a data asset, selectable in the Interior Adjustments category as "Occlusion Settings". This settings object defines things like the location occlusion traces come from, how many are used, how often occlusion is updated, etc. 
 
      You can also place the UDS Occlusion Volume actor, found in Blueprints/Occlusion, to force specific spaces to be fully occluded and always apply the interior adjustments. Note the setting on UDS for Occlusion Sampling Mode which determines if occlusion is determined by the traces for collision, the occlusion volumes, or both. 
 
-----
SUN LENS FLARE
-----
 
      UDS has its own feature to apply a filmic lens flare to the sun specifically. Note that this is a completely separate effect from the image based lens flares built into the engine. 
 
      To enable this lens flare, just check "Enable Sun Lens Flare" in the Lens Flare category, and select the Lens Flare Type you want. 
 
      It is possible to build your own custom lens flare type using this same setup. In the advanced dropdown of the Lens Flare category, see the option "Custom Lens Flare Parent Instance". To use this, create an instance of one of the existing lens flare material instances in Materials/Material_Instances/Lens_Flares. You can adjust the textures, colors, and mapping parameters of all the lens flare elements in your custom instance and use it with that exposed Custom parent instance setting. 
 
-----
AURORA
-----
 
      UDS has two ways of rendering aurora effects in the sky, both adjustable from the Aurora category: 
 
      2D Aurora 
 
      This is the effect which will be present if you enable "Use Auroras" in the Aurora category, and the Sky Mode is not Volumetric Aurora and is one which is compatible with 2D aurora. These 2D Aurora effects are a simple shader effect drawn using textures in the sky material. You can find settings specific to the 2D aurora and the way they're drawn in the advanced dropdown of the Aurora category. 
 
      Volumetric Aurora 
 
      Volumetric Aurora can be selected as a sky mode in Basic Controls. This effect is rendered using a volumetric cloud component, so it's a full 3D effect which the camera can go into/above. It is also affected by the Volumetric Cloud Rendering Mode setting in the Volumetric Clouds category. 
 
      Both of these effects share most of the settings in the Aurora category, for colors and movement. 
 
-----
PAINTING VOLUMETRIC CLOUD COVERAGE
-----
 
      There are tools included for manually painting cloud coverage into the volumetric cloud layer. This painted coverage level is applied on top of the global Cloud Coverage value set on UDS (or by the weather state if UDW exists). 
 
      With UDS in the scene and Volumetric Clouds selected for its sky mode, run the Volumetric Cloud Painter editor utility, found in the Blueprints folder. 
 
      When you press Start Painting, the tool will prepare UDS and the level for clouds to be painted. An actor called UDS Cloud Paint Container will be created in the current level to hold the painting data for the level. 
 
      Here's a description of the various parts of the cloud painter interface and how to use them: 
 
      - The top bar is where you will adjust the brush used to paint cloud coverage. You can change the texture, strength, falloff, and brush spacing. 
 
      - The viewport in the cloud painter is where you will paint brush strokes onto the cloud layer. Left click to paint, right click to drag the viewport, middle mouse click and drag to zoom in and out (or use the buttons in the lower right). You can change between three view modes by clicking View in the lower left. You can also toggle the painting viewport locking the level viewport camera to the painting area, using the lock icon there. 
 
      - On the left of the viewport are the different paint and erase modes. You can paint three levels of cloud coverage, represented on the viewport as red, green, and blue. You can also erase existing painting (paint black). 
 
      - At the bottom of the tool is the button to Save to Textures. Make sure to press this to end your painting session and save your work. It will save the painted cells to texture files. It will put these into a subfolder called UDS_LevelData, in the folder where the level is located. 
 
      The painted cell textures are drawn into a cloud coverage render target on UDS. You can find the settings for the size and resolution of that target, and how its used, on UDS in the Volumetric Cloud Painting category. 
 
-----
VOLUMETRIC CLOUD LIGHT RAYS
-----
 
      If in an overcast sky you use the Volumetric Cloud Painter to create a break in the cloud coverage, you will see an effect of light rays in the distance coming down through that hole in the coverage. That's a result of the Volumetric Cloud Light Rays, which are enabled by default, and can be adjusted in the category of the same name. 
 
      Note, the volumetric cloud light rays are a custom effect which renders rays using giant additive cards controlled by a niagara system. They aren't connected in any way to volumetric fog or screen space light shafts. 
 
      There are settings in the Volumetric Cloud Light Rays category to adjust the color and spacing of the ray cards, as well as enable "Individual Cloud Light Rays" which will try to cast rays through gaps in overall high coverage cloud formations, not just painted gaps in coverage. Note that this setting can potentially render many more ray cards, so it can have a more significant performance cost. 
 
-----
UNDERWATER CAUSTICS
-----
 
      If your level has an ocean/water level, below which you want to have light imitate the effect of caustic refraction, UDS has a feature for this. 
 
      In the Water category on UDS, you can enable "Render Water Caustics" which will apply a caustic effect to the sun and moon directional light below the height specified by Water Level. Note that if you are using the Water Level setting on UDW for its weather effects, then water level will be controlled from there. 
 
      The light will also fade down to 0 as the depth below the water reaches Deep Water Falloff, to imitate the effect of direct sunlight being completely occluded deep beneath the surface. 
 
-----
POST PROCESSING
-----
 
      If you just need to apply normal global post process settings to your level, the best way to do that is still just to use a Post Process Volume in the level. If however you want to apply post process settings which are applied dynamically depending on time or weather, UDS has a feature for that. 
 
      In the Post Processing category on UDS, you'll find an empty array called Post Process Components. If you add entries to this array, the system will create a post process component for each one. 
 
      Each entry has post process settings, where you can override and modify any post setting such as bloom settings, color grading, vignette, etc. Below the post processing settings are: 
 
      - Checkboxes to Enable/Disable this post process component based on the time of day. This will control the blend weight of the component, fading it in and out based on the current time. 
 
      -  Checkboxes to mask the component with cloud coverage, fog, dust, or interior occlusion. For example, if you checked "Mask Blend Weight When Overcast", the post settings for this component would fade away as the sky becomes overcast. 
 
      By adding and customizing the entries in this array, you can create post processing settings which change for the time and weather's specific context. 
 
-----
ADDING PLANETS / MOONS USING THE SPACE LAYER
-----
 
      UDS can render additional planets and moons, to the sky layer, correctly composited so they block stars, but are obscured behind clouds and aurora. 
 
      The place to do this is the Space Layer category on UDS. You can either manually add planets to the Planets / Moons array in that category, or you can start with one of the presets by selecting something for "Add Planet / Moon Preset". 
 
      Within the Planets / Moons array, you can customize the look and behavior of each planet. 
 
      All of the settings have tooltips, but here are a few key ones to note: 
 
      - Parent controls what the planet will move with. It can be parented to the sun, the moon, or it can be left unparented so it doesn't move, or could potentially be moved manually. 
 
      - Relative Rotation rotates the orientation of the planet, relative to what it's been parented to. 
 
      - Scale sets the visual scale of the planet. 
 
      - The Terminatior settings refer to the solar terminator, the line between the light and dark sides of the planet. 
 
      - The Light Vector controls where the light source determining the light/dark side of the planet will come from. The sun, the moon, or the custom vector. 
 
      - The Glow settings control an additional element, which renders a diffuse atmospheric glow around the planet. The brightness depends on how much of the lit side is facing the camera, and is scaled with the setting Space Glow Brightness in the Space Layer category. 
 
      The space layer draws these additional planets into the sky material using the dbuffer. So Dbuffer Decals should be enabled in your project, and need to be supported by the platform, for these planets/moons to be rendered. 
 
      Also note, Unreal 5.0 did not support mesh decal objects being sorted, so while this feature can be used in 5.0, the individual objects can't overlap each other without potential sorting errors. This isn't a problem in 5.1, where mesh decal sorting was added to the engine. In 5.1, the planets will be sorted from first to last, based on their order in the array. 
 
-----
AMBIENT SOUNDS CONTROLLED BY TIME AND WEATHER
-----
 
      If you want an ambient sound actor to adjust it's volume based on if it's day or night, or based on the weather state from Ultra Dynamic Weather, there's a class included for that. 
 
      It's called "AmbientSound-Time and Weather Controlled" and can be found in the Blueprints/Sound folder. 
 
      It's a child of the normal Ambient Sound class, so it works exactly the same. Just with additional logic to control the volume multiplier with time and weather. 
 
      At the top of the Sound category, you'll find the settings for this. There are volume multipliers for day and night, as well as ones for snowy/rainy/no weather. There's also a setting called Volume Multiplier Transition Time which controls how long it will take the volume multiplier to change when time/weather triggers a change. 
 
      If using this for outdoor ambient sounds, you may also want your sound to adjust to UDW's sound occlusion, getting quieter in interiors the same way UDW's weather sounds do. There is an intended way to do this. If you use the sound class UDS Outdoor Sounds for your ambient sounds, and you have sound occlusion enabled on UDW, then your sound volume will be affected by the sound occlusion. 
 
-----
ON SCREEN SKY CONTROLS
-----
 
      If your project needs a simple on screen interface to give the user control of time and/or weather, UDS comes with a widget built to be a good quick and clean solution for that. 
 
      It's called UDS Onscreen Controls and it can be found in the widget designer palette in the Ultra Dynamic Sky Widgets category. 
 
      By default, all of its controls will be enabled, but you can use the exposed settings on the widget details to turn off the controls you don't need. For example, you could limit it to only Show Time Control, for a simple compact control for Time of Day. 
 
      There are also settings to control the time and date format displayed, and the weather presets available in the weather controls. 
 
-----
MISCELLANEOUS
-----
 
-----
SAVING THE SKY AND WEATHER STATE FOR SAVE DATA
-----
 
      If you need to save the current state of UDS, and UDW as well if using it, for saving in save data or just preserving the state across a level load, there are functions on UDS to do that. 
 
      "Create UDS and UDW State for Saving" will package the state of both UDS and UDW into a single struct, which you can save with your save data. It contains all the variables for the time and current weather state. 
 
      "Apply Saved UDS and UDW State" will take in one of those saved state structs and apply it to UDS and UDW. This is to be used when loading a save or upon entering a new level. 
 
-----
CONFIGURING FOR PERFORMANCE
-----
 
      Here are some of the most performance relevant settings to take note of if you want to reduce the performance cost of the main UDS features. 
 
      Sky Mode 
 
      The most performance relevant setting on the whole system is probably Sky Mode. The most expensive option here is of course Volumetric Clouds. 2D Dynamic Clouds is significantly cheaper to render, and Static Clouds is somewhat cheaper still. No Clouds obviously has the most minimal cost (outside of the Space mode). 
 
      Color Mode 
 
      Color Mode can be changed to Simplified Color to reduce some of the sky rendering cost, by disabling the sky atmosphere, and using simpler material logic for the sky colors. 
 
      Project Mode 
 
      If real-time performance is at all required, make absolutely certain Project Mode is set to "Game / Real-Time". The Cinematic option is for offline rendering only, and applies quality settings at runtime which will severely impact performance. 
 
      Volumetric Clouds 
 
      Volumetric Cloud Rendering Mode - Has an effect on the cost of the cloud rendering. The default of Background Only Clouds is usually a good balance of quality and performance, but the option Clouds Render Over Opaque Object (Performance) can be a lower cost option in some contexts. 
 
      View Sample Scale and Shadow Sample Scale - Reducing these will lower the cost of the volumetric rendering, but can introduce noise and artifacts. 
 
      High Frequency Levels - Increasing this above the default of 1 will increase cost of cloud rendering. 
 
      Two Layers - This option requires a much higher sample count for the volumetric clouds to resolve both layers, as wel as a slightly more complex volumetric material. 
 
      2D Dynamic Clouds 
 
      One Cloud Layer - Enabling this will reduce the cost of the sky shader somewhat with 2D clouds. Note that One Cloud Layer is already enabled on the material level at Low material quality. 
 
      Sky Light 
 
      Sky Light Mode can have some small effect on performance, due to the cost of real time sky light capture. To eliminate that cost, but still have the sky light adapt to changing time and weather, try using the mode called "Cubemap with Dynamic Color Tinting". 
 
      Volumetric Fog 
 
      Note that enabling volumetric fog has a very signifcant rendering cost. In addition, UDS' own option to enable its global volumetric material will increase that cost somewhat. 
 
      Volumetric Fog, being a rendering feature of the engine, gets its scalability settings from the Shadow Quality scalability group. Note that by default, the engine's scalability settings for Low and Medium quality disable volumetric fog entirely. 
 
      Miscellaneous Performance Tips 
 
      - Almost all of UDS' materials respond in some way to global material quality level, reducing material complexity when using Low or Medium quality materials. 
 
      - All of the lights on UDS are movable by default, but static or stationary lighting is always going to be a more performance friendly option if it can fit your project. See the entry in this document on static lighting for more info on setting that up with UDS' lights. 
 
      - You can also set the Sky Mode to change on launch with the current Effects Quality set in the Game User Settings. For example, you could make it so that settings Medium and above use volumetric clouds, while Low will use static clouds. You can do this by enabling "Use Sky Mode Scalability Map" in Basic Controls, under the advanced dropdown, and adjusting the map. 
 
      - If you want to limit how often the directional lights on UDS update as they move, to reduce the frequency that cached directional shadows need updating, there are settings specifically for that. They are the Lights Update settings in the Animate Time of Day category. 
 
-----
SAVING AND APPLYING CONFIGURATIONS
-----
 
      It may be useful to save a full configuration of all the settings on UDS (or UDW) for reusing in other levels/projects. You can do this using the configuration manager. You can find the configuration manager in the Blueprints folder, and run it by right clicking and selecting Run Editor Utility Widget (just like this Readme). 
 
      In the configuration manager, you can select your existing saved configurations to apply to your UDS actor. And you can create new configurations using the save button in the bottom right, when no configuration is selected. 
 
      You will be prompted to name your new configuration and select a place to save it. You can either save to your project files, which will put the new config into the Ultra Dynamic Sky folder in your project. Or you can save to the engine content folder, which will put the config file into an Ultra Dynamic Sky folder there. If the config is saved in the project, it will be accessible only from this project. If the config is saved to the engine, it will be accessible by UDS in any project using that engine version. 
 
-----
CONSIDERATIONS FOR MOBILE
-----
 
      If building for mobile devices, take note of the Mobile category. This category does several things by default to adjust the sky for mobile platforms. 
 
      - Volumetric Clouds Replacement will replace volumetric clouds on mobile platforms with a different sky mode. By default the replacement is Static Clouds. 
 
      - Real Time Capture Sky Light Replacement will change the sky light mode to a replacement if the sky is using real time sky light capture (which isn't supported on mobile). By default, the replacement is the dynamic tinted sky light. Note that if Captured is selected for this option, it will use a static sky light capture, which requires recapturing. 
 
      - Adjust for Mobile Renderer will adjust a number of lighting and material settings to adjust for various features the mobile renderer doesn't support. 
 
      - Mobile Platforms allows you to customize the list of platform names which UDS will consider mobile and apply the mobile overrides to. The defaults are Android and IOS. 
***
Ultra Dynamic Weather
***
 
-----
ULTRA DYNAMIC WEATHER
-----
 
      Ultra Dynamic Weather can add rain, snow, dust, lightning, material effects and more to your scene. You can control the weather state in several different ways from random variation to directly calling a weather change. 
 
      This section of the readme will walk through setting up UDW in your level, controlling the weather state in various ways, adding material functions to your project's materials for dynamic weather effects, and more. 
 
-----
WEATHER SETUP
-----
 
-----
ADDING ULTRA DYNAMIC WEATHER TO YOUR LEVEL
-----
 
      To set up weather, first make sure you have an Ultra Dynamic Sky actor in your level. The weather system will not operate without one. If you haven't used Ultra Dynamic Sky yet, you should get started with the Sky section of this readme instead. 
 
      To add weather, simply drag an Ultra Dynamic Weather actor from the Blueprints folder into your scene. It will detect Ultra Dynamic Sky on its own and hook everything up. 
 
      By default, you should see the weather in the editor change to rain. Note that just like UDS, you might need to wait for some shaders to compile before all of the effects become visible. 
 
-----
BASIC CONTROLS
-----
 
      In the details panel with Ultra Dynamic Weather selected, the Basic Controls category is where you can change the weather settings preset the system will display in the editor and start with when the game is run. 
 
      The variable Weather will allow you to select a weather settings preset. There are a number included like Rain, Partly Cloudy, Blizzard, etc. 
 
      Basic Controls is also where you can adjust the direction of the wind, which affects weather particles as well as cloud movement. You can also adjust Cloud Speed Multiplier to change how Cloud Speed is set on UDS based on wind intensity. 
 
-----
WEATHER STATE
-----
 
-----
WHAT IS WEATHER STATE?
-----
 
      Weather State refers to the set of values which determine the current weather conditions on UDW. These are the values which comprise the weather state: 
 
      - Cloud Coverage 
 
      - Fog 
 
      - Wind Intensity 
 
      - Rain 
 
      - Snow 
 
      - Dust 
 
      - Thunder/Lightning 
 
      There's also the material state, which controls the state of the material effects specifically: 
 
      - Material Snow Coverage 
 
      - Material Wetness 
 
      - Material Dust Coverage 
 
      These are part of the weather state, but at runtime the visible material state is not always derived directly from the values in the weather state. You can read more about the specifics of this in the section on material effects. 
 
-----
WHAT DETERMINES THE WEATHER STATE?
-----
 
      The current active weather state can come from a number of different sources. Here are the places which can inform the current weather state: 
 
      - The weather settings preset selected for the Weather variable in Basic Controls. 
 
      - The Change Weather function, which transitions from the active weather state to a new preset over a specified amount of time. 
 
      - Random Weather Variation, which can change the weather periodically, with the probability of each weather preset affected by the current season. 
 
      - The Manual Weather State values, in the Manual Weather State category, used in place of the Weather variable when nothing is selected for it. 
 
      - Weather Override Volumes. This is a separate actor which can be placed into the level to apply weather that is specific to a region. 
 
      See the entries specific to each of these for more information about how to make use of them. 
 
-----
WEATHER SETTINGS PRESETS
-----
 
      The Weather Settings Presets are the preconfigured weather states like Thunderstorm, Partly Cloudy, and Blizzard. They're used for the Weather variable in Basic Controls, as well as by the Change Weather function, Random Weather Variation, and Weather Override Volumes. 
 
      You can find the included presets in Blueprints/Weather_Effects/Weather_Presets. 
 
      To make your own, a good way is to just duplicate one of the assets in that folder and adjust the settings in the new preset. Your new asset will be usable in all the same ways that the included presets are. 
 
-----
CHANGE WEATHER
-----
 
      To trigger a change of the weather state at runtime, either instantly or over a period of time, the intended way to do that is to call the Change Weather function. 
 
      When Change Weather is called, it will immediately start transitioning to the new weather preset over the transition length. 
 
      When Change Weather is called, this new weather state will be applied indefinitely, until another state change is called for. 
 
-----
RANDOM WEATHER VARIATION
-----
 
      Random Weather Variation can change the weather state periodically, selecting a new weather preset based on season and a set of probability maps. It's controlled from the Random Weather Variation category on UDW. 
 
      To enable Random Weather Variation, check the box for "Use Random Weather Variation". The setting beneath that, "Start with a Random Weather Type" will determine if the settings from Basic Controls are used when the game is started, or if the system will choose a random starting weather at runtime. 
 
      Weather Type Change Interval controls the min and max of how long the system will hold on a given weather preset before picking a new random one to transition to. Transition Length is the fraction of that interval devoted to transitioning from the old weather to the new weather. 
 
      The maps of Weather Type Probabilities contain the weather presets and a corresponding probability value. This value is proportional to the rest of the values in the map. Meaning, if one type is set to 5 and the rest are set to 1, that type will be 5 times more likely to be picked. The map used will depend on the current season, controlled from the Season category. 
 
      You can also apply a climate preset from here, which will use real world climate data to create probability maps which suit that climate, and also set the temperature ranges in the Temperature category. 
 
      You can use the Change Weather function at runtime to change from Random Weather Variation to a specific static preset. If you need to change back to the Random Weather Variation, call "Change to Random Weather". 
 
-----
SEASONS
-----
 
      In the Season category, you'll find settings for how UDW determines the current season. 
 
      Season is a float from 0 to 4, representing the progression from each season to the next, starting with Spring. So for example, 0 is Spring, 0.5 is halfway between Spring and Summer, 2 is Autumn, etc. And 4 is Spring again, completing the cycle. 
 
      In the Season category, the Season Mode variable determines if the Season is derived from the Date on UDS (entered in Simulation category and incremented with each day that passes). Otherwise, the season variable is left uncontrolled, for you to set from your own classes as needed for your project. 
 
      If deriving the season from the date, you can use the Season Day Offset to change what date will correspond to what season value. 
 
      It might be useful for your materials to sample the current season, for example to make foliage change color in Autumn. There is a material function included for this. It's called "Sample UDW Seasons". It outputs a 0-1 value for each season, so they can be sampled individually for effects specific to each. 
 
-----
MANUAL WEATHER STATE
-----
 
      Much of the weather system is designed around the weather preset assets, to simplify controlling the weather state. But you can also control weather state's individual values without relying on preset assets. The way to do this is using the Manual Weather State. 
 
      To do this, first clear the Weather setting in Basic Controls, so no preset asset is selected. When this is the case, the Manual Weather State will be used instead. 
 
      The Manual Weather State category below Basic Controls is the place where that manual state is controlled. 
 
      You can also use the Manual Weather State with the Change Weather function at runtime if needed. Just call Change Weather without a preset selected for its input, and the weather will transition to the manual state instead. 
 
-----
WEATHER OVERRIDE VOLUMES
-----
 
      For large maps, it may be necessary to change weather state by region. You can do this by using a Weather Override Volume, an actor found in the Weather Effects folder in Blueprints. 
 
      To adjust the area affected by the volume, edit the points of the spline component. You can right click on a point on the spline to add new spline points. You can also scale the actor, but try to avoid non-uniform scaling. 
 
      Here are some of the key settings on the Weather Override Volume: 
 
      - Mode has two options. Single Weather Type will apply a static weather preset within the region. Random Weather Variation will apply random periodic changes, controlled the same way as random weather on UDW, but using its own set of probability maps. 
 
      - Transition Width controls how much space is devoted to transitioning from the outside weather state to the inside weather state. The transition is completely spatial, so ideally the transition width should be very high, to offer a smooth transition between states as the player crosses into the region. The default transition width is small to keep the initial size of the actor reasonable as a starting point. An ideal transition width would typically be much larger. 
 
      - Priority will determine the behavior when multiple volume actors overlap each other. 
 
      - The Temperature category contains the temperature ranges used by UDW when the player is inside the region. 
 
      - You can also apply a climate preset, and it will work the same as applying one to UDS, changing the probability maps and the WOV's temperature ranges. 
 
      Note that because the Weather Override Volume's effects are applied based on the player position, they're only visible at runtime. 
 
      By default, UDW will test against the Weather Override Volumes using the current player pawn location. You can adjust this behavior from the advanced dropdown of UDW's Basic Controls. The Control Point Source Location is what determines this. 
 
      The Weather Override Volumes will, by default, draw their material state into a render target sampled by the weather material functions, so that for example a snowy region can appear snowy from outside, and from inside you can see the lack of snow coverage outside the region. This behavior can be disabled or adjusted on UDW, from the Weather Override Volumes category there. 
 
      Weather Override Volumes also have functions which can be called to change their state at runtime, similar to UDW. There is a Change Weather function, which will transition to a specific static weather preset. And there is Change to Random Weather, which will transition over to random weather if the existing mode is Single Weather Type. 
 
      If you want to use a Weather Override Volume to change anything other than the weather state on UDW, that is possible. To do this, create a child of the Weather Override Volume class, and on that child override the function called "Custom Volume Behavior". This function is run from UDW when it uses the WOV to affect local weather. The Alpha input represents how much the player is inside the volume, from 0 to 1. 
 
-----
WEATHER ABOVE VOLUMETRIC CLOUDS
-----
 
      If Volumetric Clouds is selected as the Sky Mode on UDS, by default UDW will apply adjustments to the weather state at runtime when the player goes above the cloud layer. 
 
      You can find the settings for this in the Weather Above Volumetric Clouds category on UDW. You can enable or disable the adjustments entirely, or change the multipliers for each weather value used when above the clouds. 
 
-----
WEATHER EFFECTS
-----
 
-----
ADJUSTING RAIN, SNOW, AND DUST PARTICLES
-----
 
      There are categories on UDW for Rain Particles, Snow Particles, Dust Particles, and Shared settings which apply to all three. This is where you can enable or disable the particles of each type, and adjust their behavior and appearance. 
 
      Each emitter spawns particles in an area around the player camera, either in editor or in game. There are settings for how big this spawning area is and how many particles should be spawned. 
 
      The amount which each particle system will spawn depends on the Rain, Snow, and Dust values in the current Weather State. 
 
-----
WEATHER PARTICLE COLLISION
-----
 
      The weather particles on UDW (rain, snow, dust) have a system of particle collision, so they can hit surfaces and stay out of interior spaces. This collision is enabled by default. 
 
      The weather particles will be blocked by simple collision which responds to queries and blocks a specific collision channel. By default this is Visibility, but can be adjusted with the setting Weather Particle Collision Channel in the Shared Particle Settings category. 
 
      Note that each weather particle just queries collision once, on the frame it's spawned. This means particle collision is necessarily somewhat latent, so more suitable for objects which don't move, or only move slowly. 
 
      If you want to block a space from letting weather particles inside, without any meshes involved, you can use the UDS Occlusion Volume actor, which has a setting to make it block weather particles. 
 
-----
LIGHTNING
-----
 
      There are two types of lightning in UDW. Lightning Flashes and Obscured Lightning. Lightning Flashes are the big lightning bolts which can cast light into the scene and have a directly associated loud crash of thunder. Obscured Lightning is the particle effect which looks like flashes of lightning hidden in the cloud layer. 
 
      Note that the Lightning Flashes are spawned and animated at runtime, so they won't be visible until you play the level. 
 
      In the Lightning category, you can adjust the interval between Lightning Flashes and the frequency of Obscured Lightning. You can adjust or disable the directional light source for Lightning Flashes and the color of the lights and effects. 
 
      The amount of lightning of both types will scale with the Thunder/Lightning value of the current weather state. 
 
      If you need to manually trigger a Lightning Flash, you can do this. Call the function Flash Lightning on UDW to spawn a single flash at runtime. It has an input to enable a custom lightning location as well. Note that this is the position of the root of the lightning bolt, so it should be at cloud level. If Custom Target Location is a value other than 0, that will be the location the lightning bolt ends at. 
 
-----
MATERIAL EFFECTS
-----
 
-----
MATERIAL EFFECTS OVERVIEW
-----
 
      Ultra Dynamic Weather can control material functions which make your materials respond to the weather's changing material state. Here are the material functions included: 
 
      - Surface Weather Effects - For adding wetness, snow, and dust to solid surfaces. Intended for things like props and scenery objects, but not large ground/landscape materials. Can also render animated dripping and droplets effects on wet surfaces. 
 
      - Dynamic Landscape Weather Effects - Can add both dynamic snow/dust coverage and wetness/puddles to ground materials, particularly landscape materials. The snow/dust and puddles can respond to interactions caused by a special component placed on moving objects. 
 
      - Glass Window Rain Drips - Intended for use on flat transparent glass surfaces. Renders falling water droplets which form in rain. 
 
      - Foliage Wind Movement - Outputs a world position offset which can deform a mesh using simple noise based animation, scaled by the current wind intensity. Intended for foliage models mainly. 
 
      - Water Surface Rain Ripples - Adds animated raindrop impacts to the normal of a water surface. 
 
      The behavior of the material state can be adjusted from the Material Effects category on UDW. A key setting here is Material Effects Take Time to Change. If that's enabled, then at runtime the material state will change over time based on the weather. For example, as it snows, material snow coverage will slowly increase. The time over which this happens for each type of weather can be found in the Material Effects category as well. 
 
      If you instead disable Material Effects Take Time to Change, then the material state at runtime will come directly from the current weather state. So, for example, if you suddenly changed from Snow to Partly Cloudy, the material snow coverage would disappear instantly, because it's directly using the material snow coverage value from the Partly Cloudy weather state. 
 
      There are also material functions included called Wet Weather Effects and Snow and Dust Weather Effects. These are included for legacy support only. In all cases where they might be used, I would recommend using the newer Surface Weather Effects function instead. 
 
-----
SURFACE WEATHER EFFECTS
-----
 
      Surface Weather Effects is a material function which can make a solid surface appear wet and gather snow and dust coverage, as controlled by the current material state on UDW. It can also apply optional effects for dripping and small droplets on wet surfaces. 
 
      Surface Weather Effects is intended for props, architecture, and other solid scenery objects. It's not intended for expansive ground surfaces like landscapes. For that, I'd suggest using the Dynamic Landscape Weather Effects. 
 
      You can see an example of a material using Surface Weather Effects in the UDS demo map. 
 
      Setup 
 
      To add Surface Weather Effects to a material, search the material palette for Weather and select Surface Weather Effects. 
 
      You should typically place the function right at the end of your material graph, before the material result. 
 
      Surface Weather Effects uses a Material Attributes input and output. This is a pin which combines together all possible material attributes into one. If your material doesn't use one of these for its result, you can use Make Material Attributes and Break Material Attributes nodes to adjust for that. 
 
      Use Static Bool nodes to provide True or False inputs for Apply Wetness and Apply Snow / Dust. This tells the function which effects you want to be applied to the surface. For example, if your project will only ever need wet materials but not snow or dust, you could plug True into Apply Wetness and False into Apply Snow / Dust, to save on material complexity. 
 
      Note that by default the textures used for the effects will be mapped using world space coordinates. This is a good fit for static scenery/architecture. But if your material is going to be used on a moving object or a skeletal mesh, you should plug True into Local Space or For Skeletal Mesh, to configure the function for those cases. 
 
      Dripping and Droplets 
 
      To enable additional effects on wet surfaces, you can plug True into the Dripping and Droplets inputs. 
 
      Dripping enables an effect of rivulets of water moving down the surface where wet. 
 
      Droplets enables an effect of very small water droplets which cling to a smooth surface. 
 
      Both of these effects are influenced by Porosity. This input generally represents how porous/permeable the surface is. 0 would be a surface like car paint or smooth plastic. 1 would be something like rough concrete or sand. Without an input, Porosity will be derived from the input roughness. 
 
-----
DYNAMIC LANDSCAPE WEATHER EFFECTS
-----
 
      Dynamic Landscape Weather Effects is a material function which can add dynamic snow and dust coverage and wetness/puddles to a landscape (or general ground meshes). The setup process is a little more involved than the basic weather effects functions. You can find a basic example of a material using DLWE in the UDS demo map. 
 
      Setup 
 
      In your material, search the material palette for Weather and select Dynamic Landscape Weather Effects. 
 
      Typically, you'll want to add the function in at the end of the material graph, right before the material result. 
 
      The Dynamic Landscape Weather Effects will modify the Base Color, Metallic, Specular, Roughness, Normal, and World Position Offset. It also outputs Pixel Depth Offset. Route these attributes through the function and into the material result. 
 
      Some landscape materials make use of a single material attributes pin for their output. To account for this, just use Break Material Attributes and Make Material Attributes nodes to separate the attributes, apply the DLWE function, then recombine for the material result. 
 
      On the material instance, you can disable any features of the DLWE effects which you don't need using static switch parameters. 
 
      With the changes to the material made, go to UDW, to its Dynamic Landscape Weather Effects category, and check the option for "Enable Dynamic Landscape Weather Effects". 
 
      Interactions 
 
       Dynamic Landscape Weather Effects supports any kind of moving actor creating trails in the snow or dust and ripples in the puddles at runtime. 
 
      The way to do this is using the DLWE Interaction component. This is a scene component which you can parent to any moving component on an actor, or parent to a bone on a skeletal mesh. 
 
      For example, to make a character leave trails in snow with their feet, you would add a DLWE Interaction component for each foot, one parented to the left foot socket and one on the right foot socket. 
 
      A key setting on the DLWE Interaction component is the Size. This determines both how far from the component interaction effects will be caused, and how big those effects will be. The Size should match the general diameter of the object it represents. 
 
      The DLWE Interaction component also references a DLWE Interaction Settings asset, which contains the complete configuration of how the interaction component will behave and what effects it will use. You can use the default settings asset or create your own to configure the specifics of interaction behavior. 
 
      Render Target Area 
 
      The DLWE interactions can only happen within the space around the player covered by the render target which the interactions draw to. The scale of that area can be adjusted on UDW with the Render Target Area settings in the DLWE category. 
 
      The render target will dynamically recenter itself as your player pawn moves through the level. If you want this recentering to be based on a different location, you can do that. In UDW's Basic Controls, in the advanced dropdown, the setting is Control Point Location Source. This also affects how weather is changed by Weather Override Volumes. By default the player pawn location is used, and should be a good fit for most projects. But you can change this to use the player camera location, or to control it manually using a vector variable. 
 
      Blocking Interaction Effects with Physical Materials 
 
      It might be necessary to keep interaction sound effects and particles from happening on certain surfaces. You can use physical materials to do this. Find the arrays on UDW, in the advanced dropdown of the DLWE category, called "Physical Materials Which Disable Snow/ Dust Sounds And Particles" and "Physical Materials Which Disable Puddle Sounds And Particles". Any phys mats in these arrays will be blocked from causing interaction effects. 
 
      Using DLWE on Non Landscape Meshes 
 
      The effects are designed primarily for landscapes, but you can use them on a static mesh as well. To tell UDW and the Interaction components what surfaces the components should respond to (other than landscapes) you'll need to add physical materials to an array on UDW called "Physical Materials which Enable DLWE Interactions on Non-Landscapes". Make sure your static meshes are using physmats in that array, and the DLWE Interaction components will respond to their collision as they do to landscapes. Also, make sure your static mesh is of the object type for "Landscape Object Type" set on 
UDW. By default this is World Static. 
 
-----
GLASS WINDOW RAIN DRIPS
-----
 
      Glass Window Rain Drips is a material function which can add refractive water droplets dripping down a thin glass surface like a window in response to rain on UDW. It is made to be used on translucent materials only. 
 
      Setup 
 
      To add Glass Window Rain Drips to your material, search the material palette for Weather and select Glass Window Rain Drips. 
 
      The material function modifies the Base Color and Opacity, and outputs Base Color, Emissive Color and Opacity. Only input what attributes your material is already using, but make sure to use all three of the outputs for your material result. 
 
      The UVs used for the drip textures will be simple world space position based UVs by default, but you can replace them using the UVs input. This is recommended if using the material on any kind of moving surface, as world space UVs will not move with the mesh. 
 
-----
FOLIAGE WIND MOVEMENT
-----
 
      Foliage Wind Movement is a material function which outputs world position offset to add simple noise based movement to a material, based on the wind intensity and direction on UDW. It's intended primarily for foliage models. 
 
      Note, if working with foliage models which have their own wind movement setup already, it may be a better and simpler option to just modify those existing wind effects to reference the state of UDW's wind instead. You can use the Sample UDW Wind function to get parameters for the wind state and use those values to replace the parameters used by the existing effects. 
 
      Wind Movement Overview 
 
      The movement generated by the function is split into three layers: 
 
      - Small Movement is a high frequency layer for features like leaves, grass blades, and the ends of branches. 
 
      - Medium Movement is a middle frequency layer made to affect larger features like whole branches. 
 
      - Large Movement is a low frequency swaying movement which bends the entire mesh with gusts of the wind. 
 
      All three are enabled by default, but they can be individually disabled using the Apply inputs on the function. 
 
      Masking Movement Layers 
 
      Each of the three movement layers has an input to mask the layer. This is a value from 0-1, multiplied over the strength of the movement layer. 
 
      Typically, foliage models are authored with vertex color channels for masking levels of movement like this, but there is no common standard for what channel will be used for what kind of information. If you're using foliage models you did not author yourself, you'll need to take a look at their vertex colors to get an idea of what each channel is used for and how you might apply them to masking the three movement layers. 
 
      The Small Movement layer is the only one where a mask input is required. The Medium and Large layers will try to generate a simple mask automatically if one isn't supplied, using position relative to the mesh's pivot, but this auto mask doesn't know anything about the structure of the specific model, so a vertex color channel authored for this kind of masking will always be better to use. 
 
      The inputs for each movement layer can be used to customize the movement logic used for each. When adding this function to a foliage master material, I'd recommend using parameters for some of these inputs, so you can toggle and adjust the layers used on material instances for specific models/types of foliage. 
 
-----
WATER SURFACE RAIN RIPPLES
-----
 
      Water Surface Rain Ripples is a material function which adds ripples to the normal of a water surface, based on the amount of rain on UDW. 
 
      Setup 
 
      To add Water Surface Rain Ripples to your material, search the material palette for Weather and select Water Surface Rain Ripples. 
 
      Typically you should put the function at the very end of the material graph, right before the material result. This function just modifies the normal attribute, so you should only need to give it your material's normal as an input and plug its normal output into the material result. 
 
      The two additional inputs can be used to scale the ripple effect texture, and adjust how intense it is in affecting the normal. 
 
-----
WET WEATHER EFFECTS (LEGACY)
-----
 
      NOTE: Wet Weather Effects is now being phased out. I would recommend using the Surface Weather Effects function instead. See the entry on Surface Weather Effects for info on that setup. 
 
      Wet Weather Effects is a material function which can make a surface appear wet, as controlled by the current material state on UDW. It can also apply a dripping effect to vertical surfaces and a simple puddle effect. 
 
      Setup 
 
      To add Wet Weather Effects to a material, search the material palette for Weather and select Wet Weather Effects. 
 
      You should typically place the function right at the end of your material graph, before the material result. 
 
      Wet Weather Effects can affect Base Color, Roughness, and Normal, so route those attributes through the function using its inputs and outputs. You can safely ignore the inputs for attributes your material doesn't use. 
 
      A key input is Height, which determines which areas will collect wetness first. This input expects a 0-1 single channel value. Low values will become wet first, and high values last. You can use a constant value for Height, but the whole surface will be affected uniformly. 
 
      Dripping Effect 
 
      You can enable simple animated drips on vertical surfaces by plugging a static bool of True into Apply Dripping Effect. 
 
      By default, these dripping textures will be mapped in world space, and scaled with Drips Scale. You can instead plug False into Map Drips in World Space and supply a UV set into Drip UVs. This is especially important to do on any kind of moving object, as the world space drips won't move with the mesh. 
 
      Puddles 
 
      Similar to the dripping effect, you can turn on puddles by plugging True into Apply Puddles. Puddles work by affecting the normal on areas that reach max wetness (based on the Height input) and face close to straight up, based on their vertex normal. The puddles will also show an effect of raindrop ripples when it's actively raining on UDW. 
 
-----
SNOW AND DUST WEATHER EFFECTS (LEGACY)
-----
 
      NOTE: Snow and Dust Weather Effects is now being phased out. I would recommend using the Surface Weather Effects function instead. See the entry on Surface Weather Effects for info on that setup. 
 
      Snow and Dust Weather Effects is a material function which can make a surface gather snow coverage or dust coverage, as controlled by the current material state on UDW. 
 
      Setup 
 
      To add Snow and Dust Weather Effects to a material, search the material palette for Weather and select Snow and Dust Weather Effects. 
 
      You should typically place the function right at the end of your material graph, before the material result. 
 
      Snow and Dust Weather Effects can affect Base Color, Roughness, and Normal, so route those attributes through the function using its inputs and outputs. You can safely ignore the inputs for attributes your material doesn't use. 
 
      A key input is Height, which determines which areas will gather coverage first. This input expects a 0-1 single channel value. Low values will be covered first, and high values last. You can use a constant value for Height, but the whole surface will be affected uniformly. 
 
      Snow/Dust UVs and Textures 
 
      Without any inputs, the function will use a default set of snow textures and map them using UV set 0. 
 
      You can replace the individual textures with your own texture samples or constant values using the inputs for the snow base color, normal map, roughness, and the scatter texture, which is used to add noise to the border between coverage and no coverage. 
 
      You can replace the UVs used to map the default textures using the Snow/Dust Texture UVs input. 
 
-----
CONTROLLING YOUR OWN MATERIAL EFFECTS WITH UDW
-----
 
      You may have material logic of your own which you want to control in some way using the material state, season, or wind on UDW. There are material functions intended to help with this: 
 
      Sample UDW Material State 
 
      This function gets the values for material snow coverage, material dust coverage, and material wetness. It also applies the effects of Weather Override Volumes and the Weather Mask to these values, the same way the weather effects functions do by default. 
 
      Sample UDW Seasons 
 
      This function outputs a 0-1 value for each season. As the season value changes on UDW, each of these four values will peak at 1. The four values will always total 1. 
 
      Sample UDW Wind 
 
      This function outputs various values and vectors which describe the current state of the wind, both its direction and its intensity. 
 
      You can also reference the shared material parameter collection used by UDW directly if you want. That's found in the Materials/Weather folder, and it's called Ultra Dynamic Weather Parameters. 
 
-----
MASKING MATERIAL EFFECT COVERAGE
-----
 
      By default, the material effects for snow, dust and wetness will affect everywhere globally, across the whole scene. To mask the effects in specific spaces, they also sample from a weather mask render target, which is drawn using components of two classes. 
 
      You can either use the component classes (found in Blueprints/Weather_Effects/Components) directly in your own actor classes, or you can use the included actor classes which contain one of these components (found in Blueprints/Weather_Effects).  
 
      Weather Mask Brush 
 
       The Weather Mask Brush will draw a simple shape or gradient, selectable using the variable "Brush". The brush actor can also be scaled non-uniformly and rotated on the Z axis. Because the system works by drawing these shapes into a single render target, the shapes are always projected straight up and down on the Z axis. 
 
      You can adjust how much each brush affects snow or wetness with the variables "Mask Wetness" and "Mask Snow/Dust". 
 
      There is also an option, called "Cancel Masks Above". This affects the entire rectangle the brush occupies, disabling any brush masking above the height of the actor. 
 
      Weather Mask Projection Box 
 
       The Weather Mask Projection Box captures the meshes within the box, and uses those shapes to mask weather effects straight down. You can place a projection box around a complex structure like a building, and the shapes of the structure will occlude directly below. 
 
      Try to place the box so it encompasses only the region of mesh geometry which needs to occlude downward. For example, you might place the box so it only contains the roof of a house. Also try to make sure the width and length of the box is as small as it can be to fit the occluding objects inside, to eliminate wasted space and resolution of the resulting render target. 
 
      In the exposed settings for the projection box, you can adjust the amount snow/dust and wetness are masked, as well as how much the mask beneath the box is blurred when drawn into the mask target. 
 
      If you want any individual material or instance to ignore the weather mask actors, you can use the static switch parameters "Apply Weather Mask Brushes to Wetness" and "Apply Weather Mask Brushes to Snow" to disable the effects at the material level. 
 
      The effects of the mask will stop once they reach a certain distance from the camera. This is determined by the width of the render target they draw to for the masking. You can adjust that scale in the Weather Mask Target category on UDW. 
 
-----
SOUND EFFECTS
-----
 
-----
ADJUSTING WEATHER SOUNDS
-----
 
      By default, Ultra Dynamic Weather has sound effects enabled for wind, rain and thunder. From the Sound Effects category, you can disable each type of sound, or you can adjust the volume of each as well. The resulting volume of the sounds will scale with these settings as well as the current weather state. 
 
      If you need to adjust which sounds are turned on at runtime, I'd recommend leaving the sounds which can be active enabled, and setting their Volume variables at runtime. 
 
      The Sound Effects category is also where you can adjust how lightning's accompanying thunder sound is delayed from the visual effect. By default, the delay is very slight, but you can increase it to represent a realistic speed of sound delay using the setting Cloud Thunder Delay Per KM. 
 
-----
SOUND OCCLUSION
-----
 
      The sound effects on UDW are affected by a simple system of sound occlusion by default, which periodically sends out traces from the camera location and adjusts sound volume and low pass filter frequency (how muffled the sound is) if the player is in an enclosed space. This behavior can be disabled or adjusted from the Sound Occlusion category. 
 
      Note that sound occlusion operates by sampling the location of the player camera, so it will only be applied at runtime. 
 
      The settings for the occlusion system are defined in a special data asset, selected in the Sound Occlusion category. You can use the existing standard occlusion settings asset, or make your own to customize the behavior of the occlusion system. 
 
      You can also place the UDS Occlusion Volume actor, found in Blueprints/Occlusion, to force specific spaces to be fully occluded and always attenuate weather sounds. Note the setting on UDW for Occlusion Sampling Mode which determines if occlusion is determined by the traces for collision, the occlusion volumes, or both. 
 
      If you have sound sources of your own which you want affected by UDW's sound occlusion, you can set them to use the sound class called "UDS Outdoor Sound". 
 
      Also note the component class called UDS Occlusion Portal. If any of these exist in the area around the occlusion sampling location, they will be traced against, and can negate occlusion if the trace is unobstructed. For example, you could have an occlusion portal component placed for an open door or window, allowing outside weather sound effects to enter the space in a more natural way. 
 
-----
SAMPLING WEATHER
-----
 
-----
SAMPLING THE WEATHER STATE FROM UDW
-----
 
      To sample the current weather state from your own classes, you should get the Local Weather State object off of a reference to UDW. This is a weather settings object just like the weather settings presets, but created by UDW to contain the current state used by the weather effects. 
 
      If instead of specific weather state values, you need a simple description of the current state, like "Light Rain" or "Blizzard", you should call the function "Get Display Name for Current Weather". 
 
-----
SAMPLING TEMPERATURE
-----
 
      There is a function on UDW called "Get Current Temperature", which you can call to sample the current temperature in Fahrenheit and Celsius. This value is generated in a simple and predictable way where temperature is a product of weather and never the other way around. The settings for this are found in the Temperature category on UDW. 
 
      How Temperature is Calculated 
 
      - The system starts with a value of 0, and adds to it using all the values labeled Temperature Bias in the Temperature category. These values use factors like Time of Day and weather state to add or subtract from this starting value. 
 
      - The resulting value is then mapped from a range of -1 to 1 over to the Min and Max range for the current season. 
 
      - A slowly changing random value is added to the temperature value, scaled by the Randomize Temperature setting. 
 
      - If "Apply Interior Temperature" is checked when calling Get Current Temperature, then the Interior settings will be used, using the player occlusion values created by the Sound Occlusion category. 
 
      Note, if using Weather Override Volumes, these will apply their own temperature ranges while the player is inside them. You can adjust the temperature ranges it applies from the WOV's settings, or disable this behavior there. 
 
-----
CONTROLLING ACTORS WITH WEATHER
-----
 
-----
WEATHER EVENT DISPATCHERS
-----
 
      UDW has event dispatchers which your actors can bind to to trigger events based on changes to the weather state. 
 
      - Started Raining and Finished Raining, triggered based on the amount of rain in the weather state. 
 
      - Started Snowing and Finished Snowing, which is the same thing for snow. 
 
      - Dust/Sand Forming and Dust/Sand clearing, which is the same for dust. 
 
      - Getting Cloudy and Clouds Clearing, which is based on the cloud coverage in the weather state. 
 
      - Weather Display Name Changed, which fires when the output of Get Display Name for Current Weather Changes. This is a good dispatcher to use to trigger something based on general weather changes. 
 
-----
ACTOR WEATHER STATUS
-----
 
      Actor Weather Status is a component you can add to an actor which will keep track of its direct exposure to weather, as well as its temperature and wind exposure. 
 
      At runtime, this component will use periodic traces to test the actor's weather exposure and keep track of how its temperature is affected by that exposure. All of these values can be found in the component's Status category. 
 
      The Actor Weather Status component also has its own event dispatchers, for when the actor itself is exposed to rain, or snow, or its temperature goes past thresholds for hot and cold. 
 
      You can also add dynamic material instances to an array in the component's Material Effects category, to have parameters set using the status values. These parameters are all 0-1 values and are called "Wet", "Raining", "Snowy", "Dusty", and "Wind Intensity". 
 
      Instead of using an Actor Weather Status component to always track weather state, you can also do a single query of an actor's exposure to weather by calling "Test Actor for Weather Exposure" on UDW. If you have your actor call this function, and provide a reference to itself for the Actor input, the function will output floats which describe how exposed the actor is to rain, wind, snow and dust. These values each range from 0 to 1. They scale with the intensity of the weather and the amount the actor is exposed. So for example a value of 1 for snow would mean the actor is fully exposed 
to snow at maximum intensity. 
 
-----
WIND PHYSICS FORCE
-----
 
      If you have objects which simulate physics, and you want them to be able to be pushed by the current wind on UDW, there is a component included for that. It's the Wind Physics Force component. 
 
      Just add this component as a child of any primitive component which simulates physics. At runtime, the component will query the current wind state on UDW and do line traces to see how exposed to wind the component is. 
 
      You can adjust the strength of the wind's physical force, as well as other specifics of how the component behaves, from its exposed settings. 
 
-----
ADDITIONAL FEATURES
-----
 
-----
WATER LEVEL
-----
 
      If you have a global water height in your level, like for an ocean, there are features on UDW to make the weather effects be universally affected by that water level. 
 
      The option for this is found in the Water Level category. Just enable Use Water Level and set the world Z position where the water plane sits. 
 
      Here is all the water level can affect: 
 
      - Weather particles will be kept from traveling or spawning beneath the water level. 
 
      - Sound Occlusion will be fully occluded when the player camera goes below the water level. 
 
      - Screen Droplets will turn off automatically when the camera goes underwater and the screen will be wet when the camera comes back out of the water. 
 
      - The Rainbow will be masked beneath the water level is the option for it is enabled. 
 
      - The height used for the water level by UDS' water caustics will be UDW's water level if Use Water Level is enabled on UDW. 
 
-----
RAINBOW
-----
 
      You can enable a rainbow effect as a result of the weather state, on UDW. The settings are found in the Rainbow category. 
 
      A rainbow will only be visible under these conditions: 
 
      - There is rain or fog contributing to the visibility of the rainbow. The amount each of these contributes is controlled by settings in the Rainbow category. 
 
      - The camera is directly exposed to sunlight, as affected by cloud shadows. So if the camera is in shadow from the clouds, the rainbow won't be visible. 
 
      - The sun itself needs to be low enough for the rainbow to be visible above the horizon. 
 
      There are further settings in the Rainbow category to control how visible rainbows can be above the cloud layer, or below the water level, if the water level is enabled. 
 
-----
SCREEN DROPLETS
-----
 
      Screen Droplets is an effect on UDW which can render an effect of water droplets on the screen, when the player camera is exposed to rain at runtime. 
 
      You can enable the effect from the Screen Droplets category on UDW.  
 
      The settings there can control the size of the droplets texture, the strength of the effect, and how much the camera is affected based on how much its directly facing the rain. 
 
-----
RAIN DRIP SPLINE
-----
 
      Rain Drip Spline is an actor which you can place into your level to have a line or curve spawn rain droplets which fall based on the amount of rain in UDW's weather state. This can be used to make something like the edge of a building roof appear to drip water during rain. 
 
      Rain Drip Spline can be found in Blueprints/Weather_Effects. When you drag it into your scene, it will be a small straight line, but you can edit the spline to change the length and shape of the line. Move and rotate the spline points, and add more spline points by right clicking on the spline path. 
 
      The collision and appearance of the particles this actor spawns will be controlled using the settings for rain particles on UDW. 
 
-----
CINEMATICS AND RENDERING
-----
 
-----
USING WEATHER WITH SEQUENCER
-----
 
      Here are some tips for how the weather system can be animated using sequencer. 
 
      Keyframing the Weather State 
 
      To animate the current weather state manually using sequencer, you'll want to use the Manual Weather State, keyframing the variables in that category. See the entry on the Manual Weather State for more info on using that. 
 
      Keyframing the Material Effects 
 
      If you're keyframing the Material Wetness, Material Snow Coverage, or Material Dust Coverage directly, you'll need to make one change to have those keyframes work the same at runtime as they do in your sequence. In the Material Effects category, disable "Material Effects Take Time To Change". This will make it so at runtime the material effects show exactly as they're keyframed at each frame, instead of changing gradually with weather state. 
 
      Keyframing Cloud Movement 
 
      To animate cloud movement in a way that will play back the same every time: First, set Cloud Speed to 0 on UDS. Then, turn off Randomize Cloud Formation On Run, in Cloud Movement on UDS. Then in Sequencer, keyframe the Cloud Phase variable on UDS. This will allow directly controlling cloud movement with keyframes. 
 
      Weather Particles in Sequence Editor 
 
      When previewing a sequence where UDW is bound to the sequence, note that the weather particles will not be visible during the playback preview in editor. This is just a limitation of sequencer's preview, not something which will be present at runtime/in a render. It's because the only way the sequence editor has to update bound blueprints is by re-running their construction script, which restarts niagara components. To test how particles are looking at a specific place in the animation, pause the playback there and the particles should resume. 
 
      Keyframing a Lightning Flash 
 
      Lightning flashes, being animated by blueprint logic, will not be visible when previewing sequences, though they will be visible if rendering a sequence or running one in game. If you want direct control over the timing of lightning flashes in your animation, disable Spawn Lightning Flashes on UDW, and run the UDW function Flash Lightning using a trigger event track on your sequence. The Flash Lightning function also has inputs for providing a custom location for the lightning flash. Note the location it takes should be the place the bolt starts, so it should be up at the height of the c
louds. 
 
      Rendering a Movie 
 
      Take note of the Project Mode setting on UDS, if you need to render a movie for offline playback. Change that setting on UDS to "Cinematic / Offline", to force max quality for the sky rendering and force both blueprints to update completely every frame at runtime. See the rendering entry in the sky section for more info. 
 
-----
ADVANCED TOPICS
-----
 
-----
ADDING DLWE TO A VIRTUAL HEIGHTFIELD MESH
-----
 
      There is support for using Dynamic Landscape Weather Effects on a virtual heightfield mesh. Note before starting that the Virtual Heightfield Mesh plugin, as of writing this, is still experimental and in my experience pretty difficult to get consistent results from. So understand before proceeding that even following this setup process correctly, you may encounter bugs and rendering artifacts which are due to the early and unstable state of the plugin. 
 
      The following setup process assumes the starting point is a working existing setup with a virtual heightfield mesh, that you are adding DLWE to. 
 
      Setup 
 
      - First, on your Runtime Virtual Texture asset for base color/normal/roughness/etc, make sure its Virtual Texture Content setting is "YCoCg Base Color, Normal, Roughness, Specular, Mask". This is necessary because the DLWE effects require the Mask channel to send information about the surface detail to the main pass. Make sure after doing this that you change the matching setting on the Runtime Virtual Texture Sample node in your material. 
 
      - There are two functions you'll be adding to your material. One is the normal Dynamic Landscape Weather Effects function, to render in the main pass on top of the RVT sample. The other is a special function called DLWE_VHFM_RVT_Stage, which exists to effect the output rendered to the RVT. 
 
      - Add the DLWE function to the graph, right after the Runtime Virtual Texture Sample node, using the sample outputs as the inputs for the DLWE function, and plugging the DLWE function outputs into the material result. Plug a static bool of True into the input "For Virtual Heightfield Mesh", and plug the RVT sample's Mask output into the input below that, called "Mask RVT Sample (For VHFM)". 
 
      - Add the DLWE_VHFM_RVT_Stage function to the graph, before the Runtime Virtual Texture Output node. Route your World Height through that function, so that it can change the height for the virtual heightfield mesh. Also plug Specular and Mask from the function into the RVT output. The inputs on the function for Min Height Level and Max Height Level should have values plugged in which represent how far from the landscape's height the ground is being deformed up and down by your texture detail. For example, if your displacement used the full 0 to 1 range and is being multiplied by 10 and a
dded to the landscape height, the Min Height Level would be 0 and the Max Height Level would be 10. 
 
      - On UDW, make sure "Enable Dynamic Landscape Weather Effects" is checked. 
 
      - Also check "Support Virtual Heightfield Mesh" and in the option below that for "VHFM Runtime Virtual Texture Volume", select the RVT Volume in your level which is responsible for rendering to the World Height RVT, used to generate the heightfield mesh. 
***
Common Issues
***
 
-----
TECHNICAL NOTES
-----
 
      This section is a technical overview of the main features of the Ultra Dynamic Sky and Ultra Dynamic Weather blueprints. It's intended for those who want to modify the system to change or add their own features, to give a sense of the structure of the blueprints. Unlike the rest of this readme, which is written to be relatively approachable to unreal beginners, this section assumes the reader has a good working knowledge of blueprint. 
 
-----
ULTRA DYNAMIC SKY
-----
 
-----
OVERVIEW
-----
 
      Here is a brief look at UDS' overall blueprint structure, to start off. 
 
      Construction Script 
 
       UDS' construction script logic is contained in a function called Construction Script Function, to allow other classes like UDW and the editor utilities to refresh UDS manually in the editor. 
 
      The construction script itself mainly handles setting which component is active and visible for the lights, making dynamic material instances and setting visibility for the sky and clouds, and creating arrays of those dynamic material instances for referencing elsewhere in the update functions. After that, it runs the update functions themselves. 
 
      Update Functions 
 
       Update Static Variables and Update Active Variables are the two update functions, used to apply changes to properties on the components and material instances. 
 
      Update Active Variables is for applying properties which change with time, weather, or other variables which are expected to commonly change at runtime. Update Active Variables is run frequently while the level is running, to update with these changes. 
 
      Update Static Variables is for properties which aren't expected to change at runtime. It's run as part of construction script and once on event begin play, but won't run again at runtime unless called manually. 
 
      Cache System 
 
       If Project Mode is set to Game/Real-Time, UDS utilizes a cache system which spreads out complex calculations so they don't have to happen every time Update Active Variables is run. 
 
      The function which caches these values is called Cache Properties. See the section below about the cache system for more information on how that works. 
 
      General Organization 
 
       For finding functions relevant to any of the specific features on UDS, take note first that the functions are categorized generally by feature. For example, the function which creates the Lens Flare post material, "Set Up Lens Flare" is in the Lens Flare category. 
 
      Also note that nearly every function has a description which should give a general idea of what purpose it serves and how it's used. 
 
-----
CACHE SYSTEM
-----
 
      The cache system exists to spread out calculations of changing properties, so that they don't have to run every time Update Active Variables is called. 
 
      The function which does this is called Cache Properties. It uses a set of functions to cache values of different data types to arrays of new and old values. These cached values are then lerped from old to new between updates of the cache. 
 
      The update period of the cache is not constant. It changes to be more frequent when faster changes are called for. The function which handles that is called Monitor for Changes. It looks at how time and weather variables are changing on each frame and these change speeds are used to affect cache timing immediately if necessary. The cache timing itself is handled from the function Increment Cache Timer. 
 
      The function Scaled Cache Period is what actually determines the current cache period based on the current change speeds. The max amount the cache can space out updates is exposed with the variable Max Property Cache Period. 
 
      Note, because Cache Properties operates essentially separately from the update functions, it has some of its own variables for things like Cached Sun Vector or Cached Moon Phase. Values which need to be shared by all of the cached properties, but aren't always the exact same as the actual current values for things like Sun Vector or Moon Phase. 
 
-----
TIME AND LIGHTING
-----
 
      Time is handled through manipulation of Time of Day and the Date variables. The sun and moon rotations are derived from time (unless Manual Position options are in use) and the rotations of the sun and moon are used by Cache Properties and Update Active Variables to influence properties like fog color or light intensities. 
 
      Time of Day is manipulated using both the Animate Time of Day section in Event Tick in the main event graph, and by the Time Transition system, which can be called with Transition Time of Day, and updates with Tick Time Transition. 
 
      Date is only manipulated by UDS at midnight, to increment the day forward using the function Increment Day. 
 
      Sun and Moon rotation are cached values, so their rotation is determined in the function Cache Sun and Moon Orientation. The logic used in that function will depend on if the Simulate option is true for either the sun or moon. If it is, then the function Approximate Real Sun Moon and Stars is what handles the simulated rotations based on the time, date and coordinates. 
 
      Note, this cached orientation is only applied constantly to the Sun Root and Moon Root components, arrows which hold the rotation to be used for the visuals. The lights themselves have settings (the Lights Update settings in Animate Time of Day) which allow the lights to only periodically change rotation, so the Root components act as an intermediary. 
 
      Update Active Variables will then use the forward vector of the Sun Root component and Moon Root component to determine lots of properties which change with the lights orientation. 
 
-----
CLOUD MOVEMENT
-----
 
      Most of the properties for the clouds are handled in the Update functions, but cloud movement in particular (for Volumetric Clouds and 2D Dynamic Clouds) is handled in its own function, called Set Cloud Timing. 
 
      This function operates very differently in editor vs at runtime. At runtime cloud movement is incremented every frame using cloud velocity. It does this so that clouds move smoothly as their speed and direction change at runtime. In the editor, cloud speed is set as a constant value, to preview the speed generally as it will look at runtime. 
 
      Note that the properties used for the volumetric clouds, both here and in the Update functions, are being set on a shared material parameter collection called UDS_VolumetricClouds_MPC. This is used to allow many different materials for things like cloud fog and cloud shadows to approximate cloud coverage using the same properties sampled by the visible clouds. 
 
-----
FOG COLORS AND AMBIENT LIGHT COLORS
-----
 
      The fog/ambient colors calculated by UDS are determined mainly in the function Get Current Base Fog Colors. This outputs a number of colors, for ambient and directional fog colors, as well as individual sun and moon contributions, which are used both to set color properties on the fog component itself, and also for other purposes. For example, Ambient Fog Color is used by the sky light color and intensity logic, if the Cubemap with Dynamic Color Tinting sky light mode is selected. 
 
-----
ASSET LOADING
-----
 
      Assets which UDS needs for its active features are loaded in using the function Load Required Assets, on startup. 
 
      There is also an async asset loading setup, found in the Loading graph. It's only use is for loading in nearby cell textures for the Volumetric Cloud Painter, as the camera moves through a large level. 
 
-----
REPLICATION
-----
 
      Time of Day, Date, and Moon Phase will be replicated in a multiplayer environment. (For info about weather state replication, see the UDW replication section below.) 
 
      Time of Day's replication works by only periodically updating the "Replicated Time of Day" variable, which notifies clients to update Time of Day using the replicated value, as well as day length, night length, time speed, simulation speed, and moon phase. The rest of the time Time of Day is handled locally by the client the same as on the server. These updates just periodically keep the clients synchronized. The period between updates is set with Time of Day Replication Period. 
 
-----
ULTRA DYNAMIC WEATHER
-----
 
-----
OVERVIEW
-----
 
      The blueprint structure of UDW is modeled after UDS. 
 
      Construction Script 
 
      UDW's construction script, like UDS', is contained in a function called Construction Script Function. 
 
      This function first makes sure UDW has a reference to UDS. Then it creates the weather state objects used to hold local and global weather states, as well as states used for the weather transition system. Then it caches properties and runs the update functions. 
 
      Cache System 
 
      The cache system on UDW is nearly identical to that used on UDS, and enabled based on UDS' Project Mode setting in the same way. The only difference is how the cache timing is determined, which happens using Current Local Weather Change Speed, something calculated when updating the local weather state at runtime. 
 
      Update Functions 
 
      Update Active Variables and Update Static Variables exist in much the same way as they do on UDS. 
 
      Update Active Variables handles applying changes to niagara systems, sound components, and material effects, which change with the weather. 
 
      Update Static Variables applies properties which aren't expected to change with the weather, like the max spawn distance of the weather particles for example. 
 
      Update Current Global and Local Weather State 
 
      This is a function specific to UDW, which runs before Update Active Variables. It is what determines the current weather state to be used by the active variables for things like particle behavior and sound. 
 
      General Organization 
 
      Just like on UDS, the functions on UDW are categorized generally based on the feature they relate to. Each function also has a description, explaining what it's for and where it's used. 
 
-----
WEATHER STATE
-----
 
      The weather state on UDW is determined in two stages, the Global Weather State and the Local Weather State. 
 
      Global Weather State 
 
       The global weather state is the state determined by the weather settings on UDW itself. This would be the weather settings preset, the Change Weather function, and the random weather variation. 
 
      The Change Weather transitions are handled from the Event Tick graph. It controls the Lerp to Static Settings, which represents if the system is using the random weather variation or the static weather objects. It also controls Lerp to New Settings, which is the alpha for lerping from the state when Change Weather was called over to the new state. 
 
      Update Current Global and Local Weather State starts by creating a manual weather state object if one is needed (if the Weather object is invalid), then the Global Weather State is written to using two Lerp Between Weather State nodes. 
 
      Lerp Between Weather State takes in an A and a B state and interpolates between them with an alpha. It also takes into account differences in cloud coverage and weather, so for example if lerping from Clear Skies to Thunderstorm, it will make sure not to start increasing the Rain value until there's enough cloud coverage present first. 
 
      Then the material effects are updated, using change rates for material snow coverage, wetness, and dust coverage which are cached by the cache system. 
 
      At that point, the Global Weather State is fully updated for this frame. 
 
      Local Weather State 
 
      The Local Weather State is then generated in the Apply Current/Local Weather function. 
 
      The Local Weather State is the state as affected by Weather Override Volumes, so it is the final state used by the actual weather particle effects and sounds, and used to set cloud coverage, fog, etc on UDS. 
 
      Local Weather State is also what factors in the settings which change the weather above the volumetric cloud layer. 
 
      The Apply Current/Local Weather function first copies the Global Weather State over to the Local Weather State. It then loops through the nearby Weather Override Volumes if any exist, and samples their spline with the control point location, to find the Current Volume Alpha. The Current Volume Alpha is used to lerp the Local Weather State over to the WOV's weather state. This is also where the WOV's effect on wind direction and temperature is applied, if enabled. 
 
      Next, the weather state changes above the volumetric cloud layer are applied to the local state. 
 
      At that point, the Local Weather State is finished for this frame, so the new local state is compared to what it was before to get the speed that local weather is changing. 
 
      In the Update Active Variables and Cache Properties functions, almost all of the properties set there will be sampling the values on the Local Weather State object. 
 
-----
NIAGARA SYSTEMS
-----
 
      The niagara systems used by UDW can be found in the Particles folder. 
 
      Weather Particles 
 
       The Rain, Dust, Snow, and Wind Debris systems all inherit their main CPU emitter from the Weather Particles emitter asset. 
 
      This emitter is responsible for getting the camera transform, determining the location and range to spawn particles inside, and spawning the CPU particles which query the scene collision. 
 
      The Rain and Snow systems also have GPU emitters, which inherit from the GPU Weather Particles emitter asset. These use the arrays of valid particle paths which are created by the Weather Particles emitter, to spawn additional GPU particles which follow the same proven paths, without needing to query collision again. 
 
      Lightning 
 
      The Lightning Strike niagara system is the one used for lightning flashes with a visible lightning bolt. The Obscured Lightning system is used for the passive effect of flashes which just light the clouds. 
 
      The lightning strike system uses an emitter called Lightning Path Gen to create a series of randomized points connecting the start position to the target position. Then the Lightning Bolt emitter spawns a ribbon particle which follows that path, randomly shifting the particles connecting the points. All of the other emitters are for the adjacent cloud lighting and light flare effects. 
 
      On UDW, the niagara components for the weather particles and obscured lightning are controlled mainly from the update functions. The lightning flashes are spawned from the Lightning Flash graph. 
 
-----
SOUND
-----
 
      The Audio Update function, run inside Update Active Variables, is what updates the sound components with the weather state and the sound occlusion. 
 
      The sound occlusion is calculated on the Player Occlusion component. This is the same actor component used by UDS for its interior adjustments logic. The sound occlusion values and Local Weather State are used by the Audio Update function to set parameters and volume levels on the sound components. 
 
      The sound cues, waves, and the sound class they use (UDS_Weather) can be found in the Sound folder. 
 
-----
MATERIAL EFFECTS
-----
 
      The function Update Current Material Effects is where the material state in the Global Weather State object is incremented over time, if "Materials Take Time To Change" is enabled. 
 
      The function Update Material Effect Parameters is where the material state is used to set the parameters in the shared material parameter collection. This collection is called UltraDynamicWeather_Parameters, and is found in Materials/Weather. 
 
      The DLWE material effects have their own update functions. Update DLWE Parameters sets the DLWE relevant parameters in the collection which can change at runtime. Update DLWE Snow Compressions and Update DLWE Puddle ripples are the functions where new interactions are drawn into the DLWE render target. 
 
-----
REPLICATION
-----
 
      Weather state is replicated by UDW for multiplayer. Specifically, the Global Weather State on UDW is replicated, as well as the state on Weather Override Volumes. The clients then determine their Local Weather State themselves, so each player can experience regional weather using the Weather Override Volumes, but the states inside and outside of volumes will be shared. 
 
      The global state isn't replicated directly, but by replicating the weather preset assets used by the transition system, the random weather variation, and the weather override volumes. This way the client can only recieve updates when a change of state on one of those sources starts, and handle global weather state reliably in the same way as the server between those updates. 
***
Technical Documentation
***
 
-----
COMMON ISSUES
-----
 
      This section contains solutions and workarounds for common problems faced when working with UDS and UDW. 
 
-----
LIGHTING AND POST PROCESSING
-----
 
-----
EVERYTHING RENDERING BLACK AFTER ADDING UDS
-----
 
      If after adding UDS to your scene, the entire level renders black, the cause is almost certainly exposure settings. Here's a few things to try: 
 
      - In your level, check to see if you have any post process volumes which are controlling the exposure settings. If you do, disable those overrides so UDS can control exposure. 
 
      - In the viewport, in the Lit dropdown, make sure Game Settings is checked for exposure. 
 
      - On UDS, try disabling "Apply Exposure Settings" in the exposure category. 
 
-----
AMBIENT LIGHT NOT UPDATING AS TIME OF DAY CHANGES
-----
 
      If you notice that the ambient light starts out looking correct, but as time of day changes at runtime, the ambient light doesn't update, it's almost certainly a problem with the sky light not updating. Here are a few possible causes of that: 
 
      - If you see this problem on specific platforms, it's probably because they don't support real time sky light capture. I'd recommend changing Sky Light Mode to Cubemap with Dynamic Color Tinting for a good dynamic option that works on any platform. 
 
      - If you specifically notice ambient light remaining bright as night falls, you could also just have static light from an old lighting solution present in your map. To check for that, just run a lighting build, from the Build dropdown on the main editor toolbar. 
 
-----
AMBIENT LIGHT TAKES A LONG TIME TO UPDATE
-----
 
      When the sky changes quickly, due to a fast change of Time of Day or Cloud Coverage, you might notice the ambient light in the level taking a long time to adapt to that change. 
 
      This would be an aspect of Lumen, the default global illumination method in new UE5 projects. Lumen's default settings can take a long time to adjust to changes in the scene. 
 
      You can adjust that speed from a post process volume, in its Lumen Global Illumination section. The settings would be the ones for Update Speed. Note of course that increasing the speed will increase the GPU cost of Lumen. 
 
-----
WATER/OCEAN SHADER LIGHTS INCORRECTLY IN CLOUDY WEATHER
-----
 
      If using some types of water or ocean shaders, you might notice in cloudy weather that the sun or moon directional lights appear to affect the material despite being blocked by clouds. This would be because some materials like this test the intensity and color of the active directional light without factoring in light functions. 
 
      The best workaround is to enable a setting on UDS, in Basic Controls, called "Dim Directional Lights With Cloud Coverage". This will reduce directional light intensity when cloudy to approximate the effect of the cloud shadows light function for materials which can't sample it. 
 
-----
VOLUMETRIC CLOUDS
-----
 
-----
VOLUMETRIC CLOUDS DON'T RENDER IN FRONT OF OBJECTS
-----
 
      If using the default settings, you might notice volumetric clouds appearing incorrectly behind scenery that they should block. 
 
      By default, the volumetric clouds in UDS are optimized for best fidelity in typical use cases where the clouds exist entirely as a background layer. This allows them to prioritize fidelity and temporal stability. 
 
      If you need your clouds to render over the landscape/objects in your scene, the fix is simple. In the Volumetric Clouds category on UDS, just change the Volumetric Cloud Rendering Mode to one of the settings called "Clouds Render Over Opaque Objects". 
 
-----
TRANSLUCENT MATERIALS AREN'T BLOCKED BY CLOUDS
-----
 
      Even after changing the Volumetric Cloud Rendering Mode, you may notice translucent materials still rendering in front. This is because translucent materials require additional configuration to be blocked by volumetric clouds. 
 
      On the material, in its Translucency settings, make sure both Apply Fogging and Apply Cloud Fogging are enabled. Additionally, depending on the mesh, you might want to enable Compute Fog Per Pixel, as otherwise the cloud fogging will be per vertex. 
 
      Note also that this translucency support only works with the Volumetric Cloud Rendering Modes called "Clouds Render Over Opaque Objects". It won't work with Full Cinematic Quality, as that mode disables the volumetric render target which translucent materials sample to approximate cloud fogging. 
 
-----
VOLUMETRIC CLOUD ARTIFACTS INSIDE/ABOVE LAYER
-----
 
      When going up into the volumetric cloud layer you might notice banding/noise or other light artifacts in the volumetric rendering while in the layer. 
 
      The default settings are optimized to prioritize quality from the ground. Here are a few settings to try adjusting to improve render quality within the layer: 
 
      - Increase "View Sample Count Scale" This scales the number of samples used in the volumetric rendering. Increasing it will reduce noise in the resolve and can fix some artifacts if pushed high enough. Note that this will increase the performance demand of the clouds proportional to the amount you increase the setting. 
 
      - Reduce "Tracing Max Distance". There are two settings for this, one for inside the cloud layer, and one for outside. Note that these values will only be applied with the player camera location at runtime. 
 
      - Increase "Distance to Sample Max Count". This can help in particular with the horizontal band of light that can be seen from within the cloud layer. 
 
-----
VOLUMETRIC CLOUDS LOOK BLURRY/STREAKY WHEN MOVING FAST
-----
 
      By default, the volumetric clouds use a rendering mode which resolves the render with a long trail of past frames. Because of this, when the clouds move fast, like during a time lapse, the rendering can become blurry or ghosted. 
 
      The solution is to use any of the other Volumetric Cloud Rendering Modes. You can change the mode from the Volumetric Clouds category on UDS. 
 
-----
VOLUMETRIC CLOUDS ARE TOO DARK / GRAY
-----
 
      If the volumetric clouds look strangely dark, the problem is probably the directional light affecting them. 
 
      This can happen if you're using the "Use Custom Actor" option for the sun or moon. The built in sun and moon lights have a particular setting customized which you would need to apply to your own light to have them light volumetric clouds well. The setting is "Cloud Scattered Luminance Scale". For best results I would suggest copying the exact color from UDS' directional light components. 
 
      If you haven't selected "custom actor", the problem may instead be that you have a directional light actor or component in your level that you haven't removed before adding UDS. 
 
-----
UDS SETTINGS
-----
 
-----
CANNOT ADJUST CLOUD SPEED OR DIRECTION ON UDS
-----
 
      If you're trying to adjust Cloud Speed or Cloud Direction on UDS and the value will not change, that would be because UDW is also in the level, and is taking control of those settings. UDW controls cloud movement using the current wind. 
 
      To change these from UDW, adjust the variables Cloud Speed Multiplier and Wind Direction there. 
 
-----
GENERAL RENDERING
-----
 
-----
THE SKY COLOR FLICKERS BLACK WHEN MOVING THE CAMERA
-----
 
      If you notice the color of the sky flickering to pitch black when the camera is moved in certain places in your level, you're probably encountering an engine bug with the sky atmosphere system. 
 
      The issue is caused in some circumstances by the camera going below the "ground" defined by the sky atmosphere component. 
 
      The best solution is to select the Sky Atmosphere component in UDS (found in the component list at the top of the details panel) and move it down to the lowest point in your level, below where the camera can go. 
 
-----
BLACK NOISE ARTIFACTS WITH YELLOW WARNING TEXT ON MESHES
-----
 
      If you're seeing flickering black pixel noise on certain materials, possibly with a yellow warning message visible in the black space, the problem is probably a bad implementation of pixel depth offset on the material. 
 
      The black space + yellow warning text means that the pixel is rendering nothing due to the material being broken for that pixel. This can happen due to pixel depth offset, so the first thing I would recommend is to disconnect the Pixel Depth Offset output from the material, if it has one. 
 
-----
WEATHER
-----
 
-----
WEATHER PARTICLES NOT VISIBLE AT ALL
-----
 
      If weather particles like rain and snow are not visible in your scene, regardless of the weather state, here are some things to check: 
 
      - The particle may be blocked by a source of invisible collision in your level, like a volume which has collision configured to block the channel used by weather particles. You can check to see if the particles are being blocked somehow by turning off "Particle Collision Enabled" in Shared Particle Settings on UDW. 
 
      - Make sure Raytraced Translucency is not active. You can override the translucency mode the renderer uses from a post process volume. The weather particles render as translucent sprites, which aren't something supported well by RT translucency. 
 
      - Make sure particle sprites aren't being hidden by your viewport's Show flags. In the Show dropdown there, select Use Defaults. 
 
-----
WEATHER PARTICLES NOT SHOWING IN SIMULATE MODE
-----
 
      If running in editor using the "Simulate" play mode, weather particles will probably not be visible. 
 
      This is just a limitation of Simulate mode. It plays the level, but keeps rendering through the editor camera, and in this context Niagara cannot sample the editor camera location for the weather particles to use. 
 
      Using the other Play modes should display the weather particles correctly. 
 
-----
DLWE INTERACTIONS NOT HAPPENING WHEN THEY SHOULD
-----
 
      If you've set up Dynamic Landscape Weather Effects (following the directions laid out in the DLWE section of this readme) and interactions aren't happening, here's a list of things to check: 
 
      - Make sure your landscape (or static mesh) has simple collision using the Object Type specified by "Landscape Object Type" in the DLWE settings on UDW. By default, this is World Static. Also make sure the collision is configured to respond to queries. 
 
      - Make certain "Enable Dynamic Landscape Weather Effects" is on. 
 
      - Make sure the DLWE Interaction component is positioned and has a size big enough to trigger interactions with the surface. 
 
      - Note the Control Point Location Source in the advanced dropdown of the Basic Controls category. This determines the area the DLWE effects can happen in, and by default it's centered on the player pawn location. If your camera is far from your pawn, or a pawn doesn't exist at all, you may need to change this setting. 
 
-----
WEATHER PARTICLES NOT COLLIDING WITH OBJECTS
-----
 
      If you're having trouble with weather  particles not being blocked by objects that they seemingly should, here are a few things to check: 
 
      - Make sure the object has simple collision. The traces used by the particles only check for simple collision. 
 
      - Make sure the collision profile of the object is set to block the trace channel selected on UDW as "Weather Particle Collision Channel". By default this is Visibility. Also make sure the collision response is Collision Enabled or Query Only, as queries are what the particles use. 
 
      - If particles are coming through the roof of a building with a very tall ceiling, it may be that the particles aren't checking high enough for the ceiling when they spawn in. You can adjust this distance using "Ceiling Check Height" on UDW. 
 
-----
WEATHER SOUNDS GO QUIET AT INCORRECT TIMES
-----
 
      By default, the sound effects on UDW use a simple system of sound occlusion to dim and muffle weather sounds when inside interiors. 
 
      If you're noticing this happen at incorrect times, it's probably because the occlusion traces are hitting surfaces they shouldn't. You can adjust the behavior of the occlusion system (or disable it completely) from the Sound Occlusion category on UDW. 
 
-----
SPLASH PARTICLES RENDERING WRONG IN VR
-----
 
      In VR / Stereo rendering, you might notice rain splash particles rendering only in one eye. 
 
      This is an issue with a project setting called Instanced Stereo. It doesn't seem to support GPU particles rendering as sprites which use deferred decal materials, which is how the splashes work by default. 
 
      One thing you can try is changing the Splash Particles Rendering Mode setting on UDW to "Translucent". This of course will make the particles subject to the limitations of translucent lighting, but they should render in both eyes of the HMD with instanced stereo.