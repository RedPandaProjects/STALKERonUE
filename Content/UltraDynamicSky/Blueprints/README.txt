***
PART 1
***
-----
SKY INTRO
-----
Hello! Thanks for purchasing Ultra Dynamic Sky!
This readme should cover all the basics, but if you have any questions or run into any problems, please email me at everettgunther@gmail.com
Also, there are tooltips for just about every setting on the sky and weather, so if you're confused about what something does, hover over it for a specific explanation.
-----
ADDING THE SKY TO YOUR SCENE
-----
First make sure you delete from your scene any existing lights for the sun, moon, and sky light, as well as any existing fog actors, sky spheres, or sky atmosphere actors.
If after doing this there is still some ambient light present, that probably means there's still static light in the lightmaps from the previous lighting solution, so run a lighting build (from the Build dropdown on the main toolbar) to clear that out.
Drag the Ultra_Dynamic_Sky actor into your scene from the Blueprints folder.
The default state of the sky will use the built in light components for the sun, moon and sky light, and all three will be movable by default. If you need to, you can adjust their mobility using the mobility settings in the Sun, Moon and Sky Light categories.
-----
SETTING THE TIME OF DAY
-----
Change the Time of Day property in Basic Controls to adjust the time of day the system will start with. If you need more control over the path of the Sun, you can also adjust Sun Angle and Sun Inclination in the Sun category.
The Time of Day variable is a single float from 0 to 2400. This is to simplify things like setting and animating the time. But if you need, you can access the Time at runtime as an Hours/Minutes/Seconds value using the "Get Time of Day in Real Time Format" function or as a DateTime structure using "Get Current Date and Time".
If you want to adjust the specific times that the sun comes up and goes down, you can do that with the Dawn Time and Dusk Time settings, also in Basic Controls. For setting up a realistic sun based on earth coordinates, see the section below about the Simulation category.
-----
SETTING THE SKY MODE
-----
In Basic Controls, there is a setting called Sky Mode. This controls the type of clouds (or volumetric aurora) in use. Here is a quick explanation of each option:>1. Volumetric Clouds - Enabled by default, this sky mode uses a field of full 3D volumetric clouds. This can offer the most realistic visuals, but at a performance cost that may be too much for some projects/platforms.
2. Static Clouds - Designed to mimic the look of the volumetric clouds but as a static cloud texture. Much lower performance cost than volumetric clouds. Incompatible with aurora effects. This mode does its best to adapt to cloud density so it can still be used with weather effects, but the cloud formations being static makes those transitions much less natural looking.
3. 2D Dynamic Clouds - Dynamic clouds drawn using panning 2D textures.
4. No Clouds - This mode uses no dynamic clouds at all.
6. Volumetric Aurora - Sacrifices clouds to render full 3D aurora effects. Adjustable using the settings in the Aurora category.
7. Space - For scenes set in outer space, with no planet/ground beneath the viewer. Disables all clouds, atmosphere, sky coloring, leaving only sun, moon and stars.
If you decide to use Volumetric Clouds, also take note of the setting called Volumetric Cloud Rendering Mode, in the Volumetric Clouds category. It determines how the volumetric clouds are rendered and has a large impact on performance and quality. It also determines if volumetric clouds can render over opaque objects.
-----
SETTING THE COLOR MODE
-----
The next setting in Basic Controls, called Color Mode, determines where the colors for the sky come from.>1. Sky Atmosphere - The system will use unreal's sky atmosphere rendering system to imitate correct atmospheric scattering. The settings for this can be further adjusted from the Sky Atmosphere category.
2. Simplified Color - Determines colors in simpler ways, by sampling an atmospheric scattering LUT texture, along with adjustable color values. Cheaper on the material level than the sky atmosphere, and more directly customizable. The color values used by Simplified Color can be changed from the Simplified Color category.
-----
ADJUSTING THE CLOUDS
-----
In the Basic Controls category, you can make adjustments to the cloudiness using the Cloud Coverage property.
In the Volumetric Clouds category, you can adjust settings for the 3D Volumetric Clouds. Some key things adjustable here include the altitude of the clouds, their scale, and their density.
In the 2D Dynamic Clouds category, you'll find all the settings relating to the look of the 2D clouds, from softness and shading effects to the noise texture used to generate the clouds.
In the Cloud Movement category, you can change the clouds' speed as they move across the sky. You can set the Cloud Speed variable to 0 if you want the clouds to be static. To adjust the formation of static clouds, you can use the Cloud Phase property. Note that if you have UDW also placed in your scene, the cloud speed will be controlled from there, using the current wind speed/direction, so it will not be editable on UDS.
In Cloud Shadows, you can also enable and disable the cloud shadows cast by the sun and moon. It's highly recommended to leave this enabled. You can also adjust how soft cloud shadows are and how intensely they darken the light.
-----
MANUALLY POSITIONING THE SUN AND MOON
-----
If you'd like the sun or moon to ignore time of day and position them manually in an easy way, that is possible.
For the sun, check "Manually Position Sun Target" in the Sun category.
For the moon, check "Manually Position Moon Target" in the Moon category.
Then, with UDS selected, click on the blue diamond shaped widget for the Sun Target or the Moon Target. Simply move them to reposition the sun or moon at any angle.
-----
ADJUSTING SKY ATMOSPHERE
-----
The Sky Atmosphere is what determines the coloring of the sky, if you have Color Mode set to Sky Atmosphere.
UDS by default will control some of these atmosphere settings based on current cloud coverage. You can adjust how/if it controls these settings from the Sky Atmosphere category on UDS.
To directly control all of the sky atmosphere settings yourself, look to the component list at the top of the details panel for UltraDynamicSky. Find the "SkyAtmosphere" component and select it. You should now see all of the settings for the component. The settings categories labeled "Atmosphere" are the most relevant ones for adjusting the look of the sky.
-----
ADJUSTING SIMPLIFIED COLOR
-----
The Simplified Color settings are what determines sky colors and light scattering, if you have the Color Mode called Simplified Color selected.
The color settings in Simplified Color control the base sky color, as well as the light and dark color of the 2D Dynamic Clouds and Static Clouds.
The float curve for Directional Intensity controls how much the intensity of the directional light is dimmed down as it nears the horizon.
The color curve for Directional Light Scattering controls how the directional light color is filtered as it nears the horizon.
The atmospheric scattering LUT texture is a special HDR lookup table which is used to imitate the appearance of atmospheric scattering for the sun and the moon. The Y axis represents the height of the sun or moon relative to the horizon, and the X axis is roughly the height of the specific pixel being rendered for that point in the sky. It is very specifically and carefully authored, and if you want to customize it, I would suggest exporting it and making modifications rather than trying to make your own from scratch.
-----
ADJUSTING THE MOON AND STARS
-----
You can adjust the Moon in several ways in the Moon category. You can change its scale in the sky, its inclination and angle (the path it takes across the sky), its color, and its intensity. You can also adjust the moon's phase, from 0 to 27.
The Moon category also contains settings for the moon's directional light. It's color, intensity, and if it casts shadows.
In the Stars category you can change the intensity of the stars, when they appear at night, the speed they move, and their color.
From these categories you can also select your own custom texture for the moon or the stars.
-----
SETTING UP A DAY/NIGHT CYCLE
-----
In the Animate Time of Day category, check the box labelled Animate Time of Day. When you play the game, the sky will animate indefinitely, starting at the time of day you have selected. You can change the Day Length and Night Length as well, which is entered in a value of minutes.
Note that time will only move forward if you have Movable selected for the Sun's mobility.
If you have Simulate Real Sun enabled in the Simulation category, the speed time moves is determined by Simulation Speed, not Day Length and Night Length.
-----
CONFIGURING THE SKY LIGHT
-----
In the Sky Light category, Sky Light Mode determines the source for the sky light cube map. There are three options:>1. Capture Based: The sky light will generate a cubemap by capturing the sky. By default, this method uses the real time capture option. If real time capture isn't necessary for your project, you can disable the real time capture from the Capture Based Sky Light category. Note that Real Time Capture is not supported by all platforms, and is incompatible with Volumetric Fog.
2. Custom Cubemap: A simple static cubemap texture, set using the Custom Cubemap option below.
3. Cubemap with Dynamic Color Tinting: This method uses a flat grayscale cubemap and tints it dynamically with the fog color to align well with the time of day. This is a good balanced option as its dynamic without requiring recapturing, and supports all platforms.Each Sky Light Mode has its own category of settings specific to the mode, found beneath the main Sky Light category.
-----
SIMULATING REAL SUN, MOON AND STARS USING DATE, TIME, AND COORDINATES
-----
In the Simulation category, you can enable settings which use a simplified astronomical simulation to map the position of the Sun, Moon and Stars.>• Simulate Real Sun will override the position of the sun, using a calculated position for the location, date and time provided.
• Simulate Real Moon will do the same for the Moon, and will also calculate the appropriate phase and orientation for the moon texture for the given time and location.
• Simulate Real Stars will replace the default tiling star texture with an accurate 360 degree starmap, pitched and angled correctly for the location and time. Note that the star map texture used for this is very high resolution, at 8192 x 8192. Consider that before using it on platforms which will downscale textures to a maximum size.
• Latitude and Longitude are how you'll input the desired location to be simulated. Make sure you also adjust the Time Zone, so that the Time of Day will be interpreted as expected for that location. The Time Zone is entered as a UTC offset. For example, the UTC offset for Eastern Standard Time is -5.
• Year, Month and Day control the date of the simulation. The day will automatically increment forward at midnight. Leap Years are taken into account. Also, these variables are replicated, so the current date will be shared by all clients if using networking.
• North Yaw can be adjusted to rotate the simulated compass directions clockwise. By default, +X is the direction of North.
• Simulation Speed controls how fast Time of Day is animated, if both Animate Time of Day and Simulate Real Sun are enabled. By default, the speed is set to 1, which is roughly a realistic speed.
• True Real Time can be enabled if you want your simulation to always reflect the actual current date and time at the simulated location. It does this by ignoring the Time of Day variable and instead timing the simulation using the system's reported UTC time. Note that because this accesses the current UTC time directly, it disregards the Time Zone setting.
• Apply Daylight Savings Time will change how the Time of Day is interpreted by the simulation. It will also make Time of Day change by 1 hour when the changeover happens twice a year. Because of this, if any Simulation option is enabled, all other Time of Day relevant systems will adapt to DST as well, to keep the system in sync with itself. You can adjust the time ranges of Daylight Savings Time from the advanced dropdown of the Simulation category.
• Date Controls UDW Season will adjust the Season variable on Ultra Dynamic Weather, if you have one in your map. It will do this using the current date, and the season will change with the time of year.Notes on the Accuracy of these Astronomical Calculations:>Because the complete versions of the appropritate astronomical algorithms would be very expensive to calculate with blueprint math, what I've created here is my own approximation. It leaves out complicating nuances such as the eccentricity of the earth's orbit, or the irregularity of the earth's shape. That said, I've done a lot of back 
and forth testing, and my approximations of the sun's position were never more than a little over a degree off from the results given by the complete algorithm, and that deviation is a worst case scenario. Typically the deviation was only in fractions of a degree. Again, based on my own tests. You may find edge cases where the deviation is worse. Still, I think the accuracy of the sun is sufficient for almost all practical applications.
The accuracy of the Moon's position is more variable than the sun's. The Moon's orbit is calibrated using it's position in 2017. So, any date within several decades of that should be generally accurate. At the very least, the moon should generally appear in the correct constellation and have the right phase.
The stars position should actually be pretty much completely accurate. This is because the north pole of the Earth and the celestial north pole are the same thing, making the calculations much simpler to have total accuracy.
-----
ANIMATING THE SKY WITH SEQUENCER
-----
Here are some tips for animating UDS using sequencer.Keyframing Time of Day> To animate Time of Day directly, you should disable the "Animate Time of Day" setting on UDS, and keyframe the Time of Day variable. When animating with the Time of Day variable, it's okay to go past 2400. The system will understand this and proceed forward in time with a new day.Keyframing Cloud Coverage> To animate the cloud coverage, you can keyframe the Cloud Coverage variable on UDS, but only if you don't have an Ultra Dynamic Weather actor in your scene. If you do, you should keyframe the Cloud Coverage variable
 on UDW instead. (See the sequencer section in Weather Basics for info about animating on UDW)Keyframing Cloud Movement> To directly animate cloud movement, first set Cloud Speed to 0 (or set Cloud Speed Multiplier to 0 on UDW if you have a UDW actor in your scene). Disable "Randomize Cloud Formation on Run" and "Clouds Move with Time of Day" as well. Then in sequencer, animate using the Cloud Phase variable. This will allow you to directly control cloud movement in a way that will play back the same way every time.Rendering a Movie>If you're rendering out a movie using sequencer, make sure to
 change Project Mode on UDS to "Cinematic / Offline". This configures the sky for maximum quality while rendering, disabling many optimizations that are useful only in real-time rendering scenarios. If rendering a movie using the path tracer, also take note of the option on UDS, "Adjust for Path Tracer".
If your project does need real-time performance at runtime normally , be sure to turn Project Mode back to Game / Real-time after completing your movie render, or game performance will be severely compromised.For tips about animating weather, see the section about sequencer in the Weather Basics section of the readme.
-----
PAINTING VOLUMETRIC CLOUD COVERAGE
-----
There are tools included for manually painting cloud coverage into the volumetric cloud layer. This painted coverage level is applied on top of the global cloud coverage value set on UDS (or by the weather state if UDW exists).
With UDS in the scene and Volumetric Clouds selected for its sky mode, run the Volumetric Cloud Painter editor utility, found in the Blueprints folder.
When you press Start Painting, the tool will prepare UDS and the level for clouds to be painted. An actor called UDS Cloud Paint Container will be created in the current level to hold the painting data for the level.
Here's a description of the various parts of the cloud painter interface and how to use them:>- The top bar is where you will adjust the brush used to paint cloud coverage. You can change the texture, strength, falloff, and brush spacing.
- The viewport in the cloud painter is where you will paint brush strokes onto the cloud layer. Left click to paint, right click to drag the viewport, middle mouse click and drag to zoom in and out (or use the buttons in the lower right). You can change between three view modes by clicking View in the lower left. You can also toggle the painting viewport locking the level viewport camera to the painting area, using the lock icon there.
- On the left of the viewport are the different paint and erase modes. You can paint three levels of cloud coverage, represented on the viewport as red, green, and blue. You can also erase existing painting (paint black).
- At the bottom of the tool is the button to Save to Textures. Make sure to press this to end your painting session and save your work. It will save the painted cells to texture files. It will put these into a subfolder called UDS_LevelData, in the folder where the level is located.The painted cell textures are drawn into a cloud coverage render target on UDS. You can find the settings for the size and resolution of that target, and how its used, on UDS in the Volumetric Cloud Painting category.
-----
OPTIMIZING FOR PERFORMANCE
-----
With everything enabled at the highest quality, UDS can be demanding for some hardware to run. Here are some suggestions if the performance cost is too high for your project.If Using Volumetric Clouds:>• Try adjusting the View Sample Count Scale and Shadow Sample Scale. Reducing these will lower the cost of the volumetric rendering, but can introduce noise and artifacts.
• Only use one layer of volumetric clouds. The option to use 2 layers is disabled by default because it has a significant performance cost.If Using 2D Dynamic Clouds:>• Try enabling the option to use only one 2D cloud layer. This can reduce the cost of the sky shader.Misc Performance Tips>• Turn off Aurora effects in the Aurora category. These are actually disabled by default, as they add a signifigant instruction count cost to the shader. If performance is a priority, consider disabling these.
• Change the Color Mode on UDS to Simplified Color. This eliminates the cost of the Sky Atmosphere and makes a signficant difference to the instruction cost of the sky material.
• Reduce the "Material Quality Level" scalability setting. The sky shader will reduce some minor aspects of its visuals based on Material Quality Level, including forcing a single cloud layer when quality is set to low. The Volumetric Clouds change significantly with Material Quality Level as well, changing the resolution of 3D noise used as well as the number of texture layers sampled for both 2D and 3D layers of the shader.
• All of the lights on UDS are movable by default, but static or stationary lighting is always going to be a more performance friendly option if it can fit your project. You can adjust the mobility of each light from their respective categories in the UDS details.
• You can also set the Sky Mode to change on launch with the current Effects Quality set in the Game User Settings. For example, you could make it so that settings Medium and above use volumetric clouds, while Low will use static clouds. You can do this by enabling "Use Sky Mode Scalability Map" in Basic Controls, under the advanced dropdown, and adjusting the map.
-----
CONSIDERATIONS FOR MOBILE
-----
If building for mobile devices, take note of the Mobile category. This category does several things by default to adjust the sky for mobile platforms.>• "Volumetric Clouds Replacement" will replace volumetric clouds on mobile platforms with a different sky mode. By default the replacement is Static Clouds.
• "Real Time Capture Sky Light Replacement" will change the sky light mode to a replacement if the sky is using real time sky light capture (which isn't supported on mobile). By default, the replacement is the dynamic tinted sky light. Note that if Captured is selected for this option, it will use a static sky light capture, which requires recapturing.
• "Adjust for Mobile Renderer" will adjust a number of lighting and material settings to adjust for various features the mobile renderer doesn't support.
• "Mobile Platforms" allows you to customize the list of platform names which UDS will consider mobile and apply the mobile overrides to. The defaults are Android and IOS.
-----
USING STATIC/STATIONARY LIGHTING
-----
To make any of your lights static or stationary, you can change their moblity using the mobility settings in the Sun, Moon and Sky Light categories.
Once lighting is built, you will still be able to make changes to the sky settings, but note that any settings which alter the lights or move them around (Like changing Time of Day) will break the lighting build and you'll have to build again. Also note that if your Sun light is set to be static/stationary, this disables the Animate Time of Day setting, as the sun can no longer be moved in game.
-----
CONTROLLING TIME OF DAY FROM ANOTHER ACTOR
-----
If you have your own system for time of day and need to control the sky externally, the process for that is very simple. Just have your actor change the Time of Day variable in Ultra_Dynamic_Sky and the sky will update to the change you make on its own. Just make sure Animate Time of Day is disabled on UDS so that the two aren't conflicting.
You can also set the Time of Day variable using different time structures, using the utility functions on UDS. "Set Time of Day Using Time Code" and "Set Date and Time" are ones to try.
-----
ADJUSTING SETTINGS INSIDE INTERIORS
-----
UDS has a category called Interior Adjustments, which if enabled, you can use to adjust things like fog and light, based on how occluded the player camera is.
To use these settings, first check "Apply Interior Adjustments" in the Interior Adjustments category.
Then, make your changes to the settings below, which modify various properties of the sky when inside an interior.
Note, the interior adjustments can only test and respond to the player camera's location at runtime, so you'll need to play the level to see the effects.
You can also place the UDS_Occlusion_Volume actor, found in Blueprints/Occlusion, to force specific spaces to be fully occluded and always apply the interior adjustments. Note the setting on UDS for "Occlusion Sampling Mode" which determines if occlusion is determined by the traces for collision, the occlusion volumes, or both.
-----
MAKING LIGHTS TURN ON AT NIGHT / DURING THE DAY
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
ADDING POST PROCESSING WHICH CHANGES WITH TIME / SKY STATE
-----
In the Post Processing category on UDS, you'll find an empty exposed array. If you add entries to this array, the system will create a post process component for each entry.
Each entry has post process settings, where you can override and modify any post setting such as bloom settings, color grading, vignette, etc. Below the post processing settings are:
1. Checkboxes to Enable/Disable this post process component based on the time of day. This will control the blend weight of the component, fading it in and out based on the current time.
2. Checkboxes to mask the component with cloud coverage, fog, dust, or interior occlusion. For example, if you checked "Mask Blend Weight When Overcast", the post settings for this component would fade away as the sky becomes overcast.
By adding entries to this array, you can customize the post processing settings for any specific condition.
-----
ADDING ADDITIONAL PLANETS / MOONS USING THE SPACE LAYER
-----
UDS can render additional planets and moons, to the sky layer, correctly composited so they block stars, but are obscured behind clouds and aurora.
The place to do this is the Space Layer category on UDS. You can either manually add planets to the Planets / Moons array in that category, or you can start with one of the presets by selecting something for "Add Planet / Moon Preset".
Within the Planets / Moons array, you can customize the look and behavior of each planet.
All of the settings have tooltips, but here are a few key ones to note:>- Parent controls what the planet will move with. It can be parented to the sun, the moon, or it can be left unparented so it doesn't move, or could potentially be moved manually.
- Relative Rotation rotates the orientation of the planet, relative to what it's been parented to.
- Scale sets the visual scale of the planet.
- The Terminatior settings refer to the solar terminator, the line between the light and dark sides of the planet.
- The Light Vector controls where the light source determining the light/dark side of the planet will come from. The sun, the moon, or the custom vector.
- The Glow settings control an additional element, which renders a diffuse atmospheric glow around the planet. The brightness depends on how much of the lit side is facing the camera, and is scaled with the setting Space Glow Brightness in the Space Layer category.The space layer draws these additional planets into the sky material using the dbuffer. So Dbuffer Decals should be enabled in your project, and need to be supported by the platform, for these planets/moons to be rendered.
Also note, Unreal 5.0 did not support mesh decal objects being sorted, so while this feature can be used in 5.0, the individual objects can't overlap each other without potential sorting errors. This isn't a problem in 5.1, where mesh decal sorting was added to the engine. In 5.1, the planets will be sorted from first to last, based on their order in the array.
-----
USING ULTRA DYNAMIC SKY WITH THE PATH TRACER
-----
If you render using the Path Tracer in a scene with Ultra Dynamic Sky, you'll face some rendering limitations of the path tracer. Specifically it cannot yet render many of the sky rendering features (clouds, sky atmosphere) which UDS makes use of. The path tracer will instead use the sky light cube map as a replacement for native sky rendering.
Ultra Dynamic Sky has a setting specifically to adjust for these limitations as well as it can, to work accepably as a background for path traced renders. Just enable the option "Adjust for Path Tracer" in the advanced dropdown of the Basic Controls category on UDS.
-----
ADDING AMBIENT SOUNDS CONTROLLED BY TIME AND WEATHER
-----
If you want an ambient sound actor to adjust it's volume based on if it's day or night, or based on the weather state from Ultra Dynamic Weather, there's a class included for that.
It's called "AmbientSound_Time_and_Weather_Controlled" and can be found in the Blueprints/Sound folder.
It's a child of the normal Ambient Sound class, so it works exactly the same. Just with additional logic to control the volume multiplier with time and weather.
At the top of the Sound category, you'll find the settings for this. There are volume multipliers for day and night, as well as ones for snowy/rainy/no weather. There's also a setting called Volume Multiplier Transition Time which controls how long it will take the volume multiplier to change when time/weather triggers a change.
If using this for outdoor ambient sounds, you may also want your sound to adjust to UDW's sound occlusion, getting quieter in interiors the same way UDW's weather sounds do. There is an intended way to do this. If you use the sound class UDS Outdoor Sounds for your ambient sounds, and you have sound occlusion enabled on UDW, then your sound volume will be affected by the sound occlusion.
-----
SAVING THE SKY AND WEATHER STATE FOR SAVE DATA
-----
If you need to save the current state of UDS, and UDW as well if using it, for saving in save data or just preserving the state across a level load, there are functions on UDS to do that.>"Create UDS and UDW State for Saving" will package the state of both UDS and UDW into a single struct, which you can save with your save data. It contains all the variables for the time and current weather state.
"Apply Saved UDS and UDW State" will take in one of those saved state structs and apply it to UDS and UDW. This is to be used when loading a save or upon entering a new level.
-----
SAVING AND APPLYING CONFIGURATIONS
-----
It may be useful to save a full configuration of all the settings on UDS for reusing in other levels/projects. You can do this using the configuration manager. You can find the configuration manager in the Blueprints folder, and run it by right clicking and selecting Run Editor Utility Widget.
In the configuration manager, you can select your existing saved configurations to apply to your UDS actor. And you can create new configurations using the save button in the bottom right, when no configuration is selected.
You will be prompted to name your new configuration and select a place to save it. You can either save to your project files, which will put the new config into the Ultra Dynamic Sky folder in your project. Or you can save to the engine content folder, which will put the config file into an Ultra Dynamic Sky folder there. If the config is saved in the project, it will be accessible only from this project. If the config is saved to the engine, it will be accessible by UDS in any project using that engine version.




***
PART 2
***
-----
WEATHER INTRO
-----
Ultra Dynamic Weather can add rain, snow, dust, sound, material effects and more to your scene. You can control the weather changes in several different ways from random variation to directly calling a weather change.
-----
SETTING UP WEATHER IN YOUR SCENE
-----
To set up weather in your scene, first make sure you have an Ultra_Dynamic_Sky actor in your scene. The weather system will not operate without one.
To add weather, simply drag an Ultra_Dynamic_Weather actor from the Blueprints folder into your scene. It will detect the Ultra Dynamic Sky on its own and hook everything up.
-----
CHANGING THE STARTING WEATHER
-----
On the Ultra_Dynamic_Weather actor, the Basic Controls category is where you can change the weather the system will display in the editor and start with when the game is run.
The variable Weather will allow you to select a weather settings object, of which there are many presets included, like Rain, Partly Cloudy, Blizzard, etc. You can easily make your own presets like these by duplicating the ones included and changing their settings.
You can also manually control the specific weather variables for the starting weather, by clearing the Weather variable, so that no preset is selected. With no preset selected, the weather state will be determined by the settings in the Manual Weather State category.
-----
ADJUSTING THE RAIN, SNOW, AND DUST PARTICLES
-----
There are categories on UDW for Rain Particles, Snow Particles, Dust Particles, and Shared settings which apply to all three. Here is where you can enable or disable the particles of each type, and adjust their behavior and appearance.
Each emitter spawns particles in an area around the player camera, either in editor or in game. There are settings for how big this spawning area is and how many particles should be spawned. There is also an option to disable the particles collision.
The collision used by the weather particles is very simple. It detects collision which blocks a specific channel specified in the Shared Particle Settings as "Weather Particle Collision Channel". By default this is Visibility, so simple collision surfaces which block visibility traces will block rain and snow.
-----
CONTROLLING THE SEASON
-----
UDW has a Season float variable which keeps track of what season it is, and its effect on the Random Weather Variation system.
The season is a float from 0 to 4, representing the progression from each season to the next, starting with spring. So for example, 0 is Spring, 0.5 is halfway between Spring and Summer, 2 is Autumn, etc. And 4 is Spring again, completing the cycle.
In the Season category, the Season Mode variable determines if the Season is derived from the Date on UDS (entered in Simulation category and incremented with each day that passes). Otherwise, the season variable is left uncontrolled, for you to set from your own classes as needed for your project.
It might be useful for your materials to sample the current season, for example to make foliage change color in Autumn. There is a material function included for this. It's called "Sample UDW Seasons". It outputs a 0-1 value for each season, so they can be sampled individually for effects specific to each.
-----
SETTING UP RANDOM WEATHER VARIATION
-----
Random Weather Variation works like this: Periodically, the system will choose a new random weather type (the kind which you select on the Weather variable in Basic Controls) and transition to it gradually. You can define everything about how it does this, from the timing of the transition and type changes to the probability of each weather type.
If you want to enable random weather variation, you'll first need to check the box for "Use Random Weather Variation". The setting beneath that, "Start with a Random Weather Type" will determine if the settings from Basic Controls are used when the game is started, or if the system will choose a random starting weather.
Season controls which of the probability maps the system will use to choose a new weather type. By default, the settings reflect a climate with snow common in the  winter and rain common in the summer.
Weather Type Change Interval controls the min and max of how long the system will hold on a given weather type before picking a new random one to transition to.
The Weather Type Probability maps contain the weather types and a corresponding probability value. This value is proportional to the rest of the values in the map. Meaning, if one type is set to 5 and the rest are set to 1, that type will be 5 times more likely to be picked.
Transition Length defines how much time is devoted to transitioning between each random type, as a fraction of the period of time before the next change. For example, if we're waiting 100 seconds until the next change and the Transition Length is 0.5, then we will spend the first 50 seconds of that time transitioning to the current type.
-----
MANUALLY CHANGING THE WEATHER STATE DURING THE GAME
-----
If you want to directly transition (either over time or instantly) the weather state to a new type of weather while the game is running, there is a function on UDW for that.
It's called Change Weather, and it has inputs for the Weather type to change to, the amount of time to transition to this new weather type. Call it at runtime on a reference to the UDW actor in the scene, to trigger a weather change.
-----
CHANGING WEATHER WITH LOCATION
-----
For large maps, it may be necessary to change weather by region. You can do this by using a Weather Override Volume, found in the Weather Effects folder in Blueprints.
A weather override volume can either apply a single weather type within its space, or it can generate its own stream of random weather variation, using its own set of probability maps. You can set which mode is used in Basic Controls on the weather override volume.
To adjust the area affected by the volume, edit the points of the spline component. You can right click on a point on the spline to add new spline points. You can also scale the actor, but try to avoid non-uniform scaling.
The variable "Transition Width" controls how much space is devoted to transitioning from the outside weather state to the inside weather state. The transition is completely spatial, so ideally the transition width should be very high, to offer a smooth transition between states as the player crosses into the volume. The default transition width is small to keep the initial size of the actor reasonable as a starting point. An ideal transition width would typically be much larger.
You can adjust the Priority variable to control what happens when multiple volumes overlap.
The effects of the Weather Override Volumes on weather state are dependent on the position of the player, so they're only visible when the game is running.
By default, UDW will test against the weather override volumes using the current player pawn location. You can adjust this behavior from the advanced dropdown of UDW's Basic Controls. The Control Point Source Location is what determines this.
The Weather Override Volumes will, by default, draw their material state into a render target sampled by the weather material functions, so that for example a snowy region can appear snowy from outside, and from inside you can see the lack of snow coverage outside the region. This behavior can be disabled or adjusted on UDW, from the Weather Override Volumes category.
Weather Override Volumes also have functions which can be called to change their state at runtime, similar to UDW. There is a Change Weather function, which will transition to a specific static weather preset. And there is Change to Random Weather, which will transition over to random weather if the existing mode is Single Weather Type.
If you want to use a Weather Override Volume to change anything other than the weather state on UDW, that is possible. To do this, create a child of the Weather Override Volume class, and on that child override the function called "Custom Volume Behavior". This function is run from UDW when it uses the WOV to affect local weather. The Alpha input represents how much the player is inside the volume, from 0 to 1.
-----
SETTING UP AND CONTROLLING MATERIAL EFFECTS
-----
Ultra Dynamic Weather can control material effects which make your materials respond to changing weather.
To use the included material effect functions, add them to your material from the Materials/Weather folder or just search for "weather" in the material function library in the material editor. There are two included basic functions. "Wet Weather Effects" and "Snow Weather Effects". (Dynamic Landscape Weather Effects is intended specifically for use on ground surfaces, and is covered in it's own section)
Wet Weather Effects can make surfaces appear wet by transitioning to a wet roughness value, appearing in the low areas of a height input first before covering the material completely. By default, it will lerp roughness to a value of 0.05 and base color will be darkened by 30%. You can adjust the specifics of how this transition looks using the inputs on the function. If you want to vary wetness across the surface, use the Max Wetness input to do so.
Wet Weather Effects also contains two extra effects which are disabled by default, but can add a lot to specific types of surfaces. One is a Puddle effect. This will modify the normals to flatten out fully wet areas and introduce a rippling texture which responds to the current amount of rain. The other extra effect is a Dripping effect. This is good for walls and vertical surfaces. It maps a dripping texture in world space to make it look like water is trickling down the surface during rain.
Snow and Dust Weather Effects operates similarly to Wet Weather Effects, but it uses a built in set of default textures instead of static values. These can be replaced with your own textures using the available inputs. Note, the reason Snow and Dust are handled as one function is because they share almost the exact same logic, so the material function just serves both purposes.
Both functions have tooltips for all their inputs which explain the particulars of how to use each one. I'd recommend reading through those for more advanced tips.
-----
SETTING UP DYNAMIC LANDSCAPE WEATHER EFFECTS
-----
Dynamic Landscape Weather Effects is a special material effect function which can add dynamic interactive snow and dust coverage and puddles to a landscape (or general ground meshes), but it requires more specific setup than the other material effects. To set this up, follow these steps:>1. Add the material function into your landscape material. The function is called "Dynamic Landscape Weather Effects", and can be found by searching the material palette. You should typically insert the function at the end of your landscape material graph, so that the weather effects will be applied on top of 
everything else. The main inputs from Base Color to World Position Offset are what the function will modify with the effects, but none are individually required for the function to work. Just supply whatever inputs your material makes use of. The rest of the inputs are for masking and customizing the effects in whatever way suits your material setup. I'd suggest reading their tooltips for more info on each.
2. Disable the effects you don't need. The material function has static switch parameters to enable/disable the Snow/Dust or Puddles and several additional effects. From your landscape material instance, disable the effects your project doesn't need, to save on material instructions and texture samples.
3. On UDW, enable the setting "Enable Dynamic Landscape Weather Effects" in the category of the same name.
4. Add interaction components to objects you want to affect snow and puddles. The component is called "DLWE Interaction". You can find it in the Blueprints/Weather Effects folder. It is a self contained blueprint component which keeps track of its movement and when appropritate will draw compressions into the snow/dust and ripples into the puddles. When adding the component, make sure to place it in the center of mass of the part of the object which will contact the ground. For example, to make a character model leave trails in the snow with their feet, you would add two DLWE Interaction compo
nents, as children of the skeletal mesh. You would set their parent socket to be the left and right foot sockets, and move the components so they sit in each foot's center of mass. When adding components, make sure to adjust their Size variable. This determines how far away the component will affect the landscape and how big the brush used for its effects. The size should be roughly the same as the diameter of the object. So for the character's feet, that would be rougly the length of the foot. By default, the interaction component will produce movement sound effects and particles. You can dis
able any of these from the component settings.For the snow and dust coverage, the depth of the trails is achieved using a simplified parallax occlusion effect. This effect is enabled by default, but can be disabled using a static switch parameter in the function.
The effects are designed primarily for landscapes, but you can use them on a static mesh as well. To tell UDW and the Interaction components what surfaces the components should respond to (other than landscapes) you'll need to add physical materials to an array on UDW called "Physical Materials which Enable DLWE Interactions on Non-Landscapes". Make sure your static meshes are using physmats in that array, and the DLWE Interaction components will respond to their collision as they do to landscapes. Also, make sure your static mesh is of the object type for "Landscape Object Type" set on UDW. B
y default this is World Static.
The render target will dynamically recenter itself as your player pawn moves through the level. If you want this recentering to be based on a different location, you can do that. In UDW's Basic Controls, in the advanced dropdown, change the Control Point Location Source. This also affects how weather is changed by weather override volumes. By default the player pawn location is used, and should be a good fit for most projects. But you can change this to use the player camera location, or to control it manually using a vector variable.
For spawning sounds and particles, the interaction components approximate some of the logic happening in the material function, to dynamically determine if a spot has puddles or snow. They don't have any way of knowing about how you're masking the effects in the material though, so you might run into situations where there are sounds and particles happening where there isn't snow or a puddle. To avoid this, you can have specific physical materials set to block these effects. In the advanced dropdown of the Dynamic Landscape Weather Effects category, find the physical material arrays for rain a
nd snow/dust. Adding to these arrays will ensure sounds and particles are not triggered on these phys mats.
-----
USING THE WEATHER MASK ACTORS TO MASK MATERIAL EFFECTS
-----
By default, the material effects for snow, dust and wetness will affect everywhere globally, across the whole scene. To mask out snow or wetness (or both) in specific spaces, there are Weather Mask Brushes and Weather Mask Projection Boxes. You can find both of these actors in the Blueprints/Weather_Effects folder.^Weather Mask Brush> The Weather Mask Brush will draw a simple shape or gradient, selectable using the variable "Brush". The brush actor can also be scaled non-uniformly and rotated on the Z axis. Because the system works by drawing these shapes into a single render target, the shape
s are always projected straight up and down on the Z axis.
You can adjust how much each brush affects snow or wetness with the variables "Mask Wetness" and "Mask Snow/Dust".
There is also an option, called "Cancel Masks Above". This affects the entire rectangle the brush occupies, disabling any brush masking above the height of the actor.^Weather Mask Projection Box> The Weather Mask Projection Box captures the meshes within the box, and uses those shapes to mask weather effects straight down. You can place a projection box around a complex structure like a building, and the shapes of the structure will occlude directly below.
Try to place the box so it encompasses only the region of space which needs to occlude downward. For example, you might place the box so it only contains the roof of a house. Also try to make sure the width and length of the box is as small as it can be to fit the occluding objects inside, to eliminate wasted space and resolution of the resulting render target.
In the exposed settings for the projection box, you can adjust the amount snow/dust and wetness are masked, as well as how much the mask beneath the box is blurred when drawn into the mask target.If you want any individual material or instance to ignore the weather mask actors, you can use the static switch parameters "Apply Weather Mask Brushes to Wetness" and "Apply Weather Mask Brushes to Snow" to disable the effects at the material level.
The effects of the mask will stop once they reach a certain distance from the camera. This is determined by the width of the render target they draw to for the masking. You can adjust that scale in the Weather Mask Target category on UDW.
-----
USING WEATHER SOUND EFFECTS
-----
By default, Ultra Dynamic Weather has sound effects enabled for wind, rain and thunder. You can completely disable the sound effects using the checkbox at the top of the Sound Effects category, or you can adjust the volume of each sound there as well.The sound effects on UDW are affected by a simple system of sound occlusion by default, which periodically sends out traces from the camera location and adjusts sound volume and low pass filter frequency if the player is in an enclosed space. This behavior can be disabled or adjusted from the Sound Occlusion category.
From this category you can also change the sound occlusion to use the Control Point Location (player pawn location by default) if that makes more sense for your project. For example, a top down game where occlusion whould be player centric instead of camera centric.
You can also place the UDS_Occlusion_Volume actor, found in Blueprints/Occlusion, to force specific spaces to be fully occluded and always attenuate weather sounds. Note the setting on UDW for "Occlusion Sampling Mode" which determines if occlusion is determined by the traces for collision, the occlusion volumes, or both.
-----
ADJUSTING THE LIGHTNING EFFECTS
-----
There are two types of lightning in Ultra Dynamic Weather. Lightning Flashes and Obscured Lightning. Lightning Flashes are the big lightning bolts which can cast light into the scene and have a directly associated loud crash of thunder. Obscured Lightning is the particle effect which looks like flashes of lightning hidden in the cloud layer.
Note that the lightning flashes are spawned and animated with runtime logic, so they won't be visible until you play the level.
In the Lightning category, you can adjust the interval between flashes of both lightning type. You can adjust the behavior of the light source for lightning flashes (if you want one at all) and the color of the lights and effects.
If you need to manually trigger a lightning flash, you can do this. Just run the function Flash Lightning. It has inputs so you can give it a custom location to spawn in. Note that this location is the position of the root of the lightning bolt, so it should be at cloud level.
-----
ADJUSTING VOLUMETRIC FOG PARTICLES
-----
Volumetric Fog Particles add an element of volumetric fog to the rain/snow particles during particularly heavy weather. This effect will only be visible if you have volumetric fog enabled on your height fog in UDS.
The "Max Fog Particle Percentage" variables control how many of the rain or snow particles will be accompanied by a fog particle. They are coupled together so that the fog particles can piggyback off of the collision calculations the rain and snow is already performing, allowing the fog particles to benefit from collision for no extra cost.
-----
CONTROLLING A WIND ACTOR
-----
You can make Ultra Dynamic Weather control a Wind Directional Source Actor using the current wind speed and direction. Just select one from the scene in the Wind Actor category. As weather changes, the speed of wind will be controlled. The amount of wind speed applied can be adjusted using a setting in that category as well.
-----
SAMPLING TEMPERATURE
-----
There is a function on UDW called "Get Current Temperature", which you can use to sample the current temperature in Fahrenheit and Celsius. This value is generated in a very simplified, approximated way, and is just meant to be used for gameplay purposes.
Here's how the temperature system works:
The system starts with a base temperature. This is determined by the current season, using the Base Temperature variables in the Temperature category.
This base value is then modified by the Factor variables in the Temperature category, using the current weather and time of day.
Then the resulting temperature is clamped inside the Valid Temperature Range.
The default values for all of these inputs are set with Fahrenheit in mind, but you can use Celsius values if you prefer. Just change the Temperature Scale at the top of the Temperature category, and supply all of the variables with your own appropriate Celsius values.
-----
ADJUSTING WEATHER ABOVE THE CLOUDS
-----
By default, UDW will apply adjustments to the local weather state at runtime, when the player camera goes above the volumetric cloud layer, if volumetric clouds are in use.
You can find the settings for this in the Weather Above Volumetric Clouds category on UDW. You can enable or disable the adjustments entirely, or change the multipliers for each weather value used when above the clouds.
-----
ENABLING A RAINBOW EFFECT
-----
You can enable a rainbow effect as an aspect of the weather state, on UDW. The settings are found in the Rainbow category.
Like in reality, rainbows will only be visible in specific circumstances:>1. There is rain or fog contributing to the visibility of the rainbow. The amount each of these contributes is controlled by settings in the Rainbow category.
2. The camera is directly exposed to sunlight, as affected by cloud shadows. So if the camera is in shadow from the clouds, the rainbow won't be visible.
3. The sun itself needs to be low enough for the rainbow to be visible above the horizon.There are further settings in the Rainbow category to control how visible rainbows can be above the cloud layer, or below the water level, if the water level is enabled.
-----
MAKING AN ACTOR DETECT EXPOSURE TO WEATHER
-----
It might be necessary for an actor to know if it's exposed to weather directly. For example, if you wanted direct exposure to snow to damage a player's stamina meter.
One way of doing this is to add the Actor Weather Status component to the actor. This component will keep track of the actor's status as its affected by weather. It will track temperature, wetness, snow, dust, and direct exposure to each weather type as well as wind. All of these values can be found in the component's Status category.
You can also add dynamic material instances to an array in the component's Material Effects category, to have parameters set using the status values. These parameters are all 0-1 values and are called "Wet", "Raining", "Snowy", "Dusty", and "Wind Intensity".
Instead of using the component to always track weather state, you can do a single query of an actor's exposure to weather by calling "Test Actor for Weather Exposure" on UDW. If you have your actor call this function, and provide a reference to itself for the Actor input, the function will output floats which describe how exposed the actor is to rain, wind, snow and dust. These values each range from 0 to 1. They scale with the intensity of the weather and the amount the actor is exposed. So for example a value of 1 for snow would mean the actor is fully exposed to snow at maximum intensity.
-----
APPLYING A PHYSICS FORCE TO A MESH USING WIND
-----
If you have objects which simulate physics, and you want them to be able to be pushed by the current wind on UDW, there is a component included for that. It's the Wind Physics Force component in the Blueprints/Weather Effects folder.
Just add this component as a child of any primitive component which simulates physics. At runtime, the component will query the current wind state on UDW and do line traces to see how exposed to wind the component is.
You can adjust the strength of the wind's physical force, as well as other specifics of how the component behaves, from its exposed settings.
-----
USING WEATHER WITH SEQUENCER
-----
Here are some tips for how the weather system can be animated using sequencer.Keyframing the Weather State>To animate the current weather state manually using sequencer, you'll want to use the Manual Weather State, keyframing the variables in that category. To enable the Manual Weather State, clear the Weather variable in Basic Controls, so that no weather preset is selected.Keyframing the Material Effects>If you're keyframing the Material Wetness, Material Snow Coverage, or Material Dust Coverage directly, you'll need to make one change to have those keyframes work the same at runtime as they
 do in your sequence. In the Material Effects category, disable "Material Effects Take Time To Change". This will make it so at runtime the material effects show exactly as they're keyframed at each frame, instead of changing gradually with weather state.Keyframing Cloud Movement>To animate cloud movement in a way that will play back the same every time: First, set Cloud Speed Multiplier to 0 on UDW. Then, turn off Randomize Cloud Formation On Run, in Cloud Movement on UDS. Then in Sequencer, keyframe the Cloud Phase variable on UDS. This will allow directly controlling cloud movement with key
frames.Weather Particles in Sequence Editor>When previewing a sequence where UDW is bound to the sequence, note that the weather particles will not be visible during the playback preview in editor. This is just a limitation of sequencer's preview, not something which will be present at runtime/in a render. It's because the only way the sequence editor has to update bound blueprints is by re-running their construction script, which restarts niagara components. To test how particles are looking at a specific place in the animation, pause the playback there and the particles should resume.Keyfram
ing a Lightning Flash>Lightning flashes, being animated by blueprint logic, will not be visible when previewing sequences, though they will be visible if rendering a sequence or running one in game. If you want direct control over the timing of lightning flashes in your animation, disable Spawn Lightning Flashes on UDW, and run the UDW function Flash Lightning using a trigger event track on your sequence. The Flash Lightning function also has inputs for providing a custom location for the lightning flash. Note the location it takes should be the place the bolt starts, so it should be up at the
 height of the clouds.Rendering a Movie>Just as with keyframing things on UDS, take note of the Project Mode setting on UDS, if you need to render a movie for offline playback. Change that setting on UDS to "Cinematic / Offline", to force max quality for the sky rendering and force both blueprints to update completely every frame at runtime.
-----
MATERIAL EFFECTS FOR SPECIAL CASES
-----
There are a few material functions which exist for special cases, to be used only on particular types of materials. Just like the basic functions, these are available by searching in the material editor for "Weather".>• WaterSurface_Rain_Ripples - This function exists to add raindrop ripple effects to a water shader. It takes in a normal value for the existing water normals and adds the raindrop effects to the output.
• GlassWindow_Rain_Drips - This function exists to add refractive water droplets dripping down a thin glass surface like a window. It is made to be used on translucent materials only.
-----
ADDING DLWE TO A VIRTUAL HEIGHTFIELD MESH
-----
There is support for using Dynamic Landscape Weather Effects on a virtual heightfield mesh. Note before starting that the Virtual Heightfield Mesh plugin, as of writing this, is still experimental and in my experience pretty difficult to get consistent results from. So understand before proceeding that even following this setup process correctly, you may encounter bugs and rendering artifacts which are due to the early and unstable state of the plugin.
The setup process (assumes the starting point is a working existing setup with a virtual heightfield mesh, that you are adding DLWE to):>- First, on your Runtime Virtual Texture asset for base color/normal/roughness/etc, make sure its Virtual Texture Content setting is "YCoCg Base Color, Normal, Roughness, Specular, Mask". This is necessary because the DLWE effects require the Mask channel to send information about the surface detail to the main pass. Make sure after doing this that you change the matching setting on the Runtime Virtual Texture Sample node in your material.
In the Landscape Material:
- There are two functions you'll be adding to your material. One is the normal Dynamic Landscape Weather Effects function, to render in the main pass on top of the RVT sample. The other is a special function called DLWE_VHFM_RVT_Stage, which exists to effect the output rendered to the RVT.
- Add the DLWE function to the graph, right after the Runtime Virtual Texture Sample node, using the sample outputs as the inputs for the DLWE function, and plugging the DLWE function outputs into the material result. Plug a static bool of True into the input "For Virtual Heightfield Mesh", and plug the RVT sample's Mask output into the input below that, called "Mask RVT Sample (For VHFM)".
- Add the DLWE_VHFM_RVT_Stage function to the graph, before the Runtime Virtual Texture Output node. Route your World Height through that function, so that it can change the height for the virtual heightfield mesh. Also plug Specular and Mask from the function into the RVT output. The inputs on the function for Min Height Level and Max Height Level should have values plugged in which represent how far from the landscape's height the ground is being deformed up and down by your texture detail. For example, if your displacement used the full 0 to 1 range and is being multiplied by 10 and added t
o the landscape height, the Min Height Level would be 0 and the Max Height Level would be 10.
- On UDW, make sure "Enable Dynamic Landscape Weather Effects" is checked.
- Also check "Support Virtual Heightfield Mesh" and in the option below that for "VHFM Runtime Virtual Texture Volume", select the RVT Volume in your level which is responsible for rendering to the World Height RVT, used to generate the heightfield mesh.




***
PART 3
***
-----
TECHNICAL DOCUMENTATION
-----
This document of technical notes will run through the major points of how Ultra Dynamic Sky's blueprint work together. It's intended for users who want to do their own modifications to expand UDS's capabilities to suit their particular project. It assumes a working knowledge of blueprint and material workflows in unreal.
First, we'll go through the blueprints major features and how and where they're controlled in the BP.
-----
BLUEPRINT - ULTRA DYNAMIC SKY
-----
Here is a rundown of the major features of the UDS blueprint, and some explanation of where to go in the BP to make changes to these behaviors.^Time of Day Management/AnimationIn the Event Graph, in the Day/Night Cycle section, the time of day is incremented if Animate Time of Day is enabled. It does this by adding the Time of Day Offset function to the existing Time of Day. It also checks to see if the day has ended (if ToD is more than 2400) and if so resets the time and triggers the Day Ended event in the same graph. Either periodically or every frame, depending on settings, the BP will run
 the Set Sun and Moon Root Rotation function, to derive the directional light rotation from time of day. As well as the “Update Directional Lights” function, to apply that rotation to the lights.^Updating Parameters and Component Settings with Time/WeatherThere are two key functions for this. One is Cache Properties, and the other is Update Active Variables. Cache Properties is what calculates the values for properties which change with time and weather. Things like fog color, light intensities, etc. These values are placed into cached property arrays periodically, to be recalled by the Update
 Active Variables function. The cached values are interpolated between as they are used in Update Active Variables, allowing them to change with time and weather and update smoothly, without needing to be recalculated completely every time they're applied.^Construction Script BehaviorUDS, on construction script, will fill all of its component references for the Sun, Moon, fog, and sky light, using the settings in Component Selection and Mobility. It will create dynamic material instances for the sky, clouds, cloud shadows, etc. And it will run the Update Active Variables and Update Static Vari
ables functions to apply the current sky state to all of the materials and components.^Sun and Moon Light ControlOn the sun and moon directional lights, the BP will control their intensity, color, cast shadows, specular scale, light function (for cloud shadows), and their source angle. This is all determined in the Update Active Variables function, in the sections labeled Sun Light Settings and Moon Light Settings.^Sky Light ControlThere are three sky light components on UDS, which might seem strange, but they exist to get around a technical limitation with the engine. The BP cannot set the So
urce Type or Real Time Capture settings on the components, so in order to support the Sky Light Mode setting easily changing modes with one step, there has to be a sky light to support all modes. The settings on the sky light which UDS controls are the cube map, the intensity, the color, and the lower hemisphere color. How it sets these depends heavily on the Sky Light Mode selected. These settings are applied in the Update Active Variables function, in the section called Sky Light Settings.^Fog ControlThe exponential height fog component on UDS is controlled using time and weather. Its densit
y, height falloff, start distance, and colors are all affected. The function which chiefly determines fog colors is the Get Current Base Fog Colors function. Fog color behavior is affected significantly by the project setting “Support Sky Atmosphere Affecting Height Fog” is enabled or not. With it disabled, fog color is entirely determined by UDS logic and its color curves for fog. If it’s enabled (and the color mode is set to Sky Atmosphere), UDS injects some color into fog for things like Night Sky Glow and Dust, but the main fog colors will come from the sky atmosphere contribution. The cha
nges to the fog settings happen in Update Active Variables, in the Height Fog Settings section.^Volumetric Cloud ControlWhen the Sky Mode is set to Volumetric Clouds, the cloud component is made visible, and UDS sets many material parameters on the volumetric cloud material, as well as settings on the volumetric cloud component. These happen in two places. First, on Update Static Variables, many settings to do with sample quality and other static adjustments to cloud features are applied, in the section labeled Volumetric Clouds. This function is run just once on startup. The dynamic changes t
o the clouds (layer height, coverage, etc) happen in Update Active Variables, in the Volumetric Clouds section there.^Cloud MovementClouds (either 2D or volumetric) move on event tick, in a function called Set Cloud Timing. It has two paths in it, one for in editor, to preview cloud movement. And one for runtime, to move incrementally in a way that can change with wind direction/speed. This function also handles the Cloud Phase, which adjusts the offset of the cloud formation in time.^Simulation FeaturesUDS can control the Sun Moon and Stars instead using an approximate simulation of real worl
d locations. This is mainly achieved in the function called Approximate Real Sun Moon and Stars. This function works by first extracting an exact UTC time (adjusting for time zone, date, leap year, etc). Then turning that exact time of year into a vector for the sun, a vector for the moon (and a phase), and an angle+axis for the real stars texture. This is sampled in the Set Sun and Moon Root Rotation function to replace the simpler logic for sun and moon movement there.^Cloud ShadowsIf cloud shadows are enabled, UDS will create dynamic material instances for the light functions on the Sun and
 Moon directional lights, and update them using the cloud movement logic, to match the cloud conditions, either with 2D clouds or volumetric clouds (cloud shadows can be used with the sky modes that don’t have dynamic clouds as well. In this case they use the 2D cloud shadows material). The cloud shadows settings are controlled in the Update Active Variables function, in the Cloud and Cloud Shadows section.^Lens FlareIf “Enable Sun Lens Flare” is enabled, UDS will enable a post process component with a special post process material (using a dynamic material instance) which renders a lens flare
 for the sun when visible. The effect is set up in the function Set Up Lens Flare, and updated periodically (to turn the effect off when unnecessary, for example when the sky is overcast) in the function Update Lens Flare.^Color ModeThere are two Color Modes selectable on UDS, which change the system significantly. The first, default mode is Sky Atmosphere, which uses a sky atmosphere component to produce sky and cloud coloring. If this mode is selected, the Sky Atmosphere component is set to be visible and the lights and materials are initialized to sample colors from the sky atmosphere. If t
he mode is instead set to Simplified Color, the Sky Atmosphere component is disabled, and color is handled manually by the UDS blueprint. The colors of the sky are set in the Simplified Color section of the Update Active Variables function. And this also has an effect on light colors/intensities, in their respective sections as well.
-----
BLUEPRINT - ULTRA DYNAMIC WEATHER
-----
Here is a rundown of the major features of the UDW blueprint, and some explanation of where to go in the BP to make changes to these behaviors.^Weather StateUDW handles weather states (the collection of weather settings like Cloud Coverage, Rain, Wind Intensity, etc) as objects of the class UDS_Weather_Settings. This allows presets for weather types and dynamic weather states to be the same object type, usable with universal functions. UDW creates its dynamic states in a function called Create Weather State Objects. The main ones of concern are the Global Weather State and the Local Weather St
ate. The Global Weather State represents the weather around the whole level, as replicated by the server if in a multiplayer setup. Local Weather State can be specific to the client/player. It’s affected locally by the Weather Override Volumes in the level.^Manual Weather StateIf the “Weather” variable is cleared (not valid), the global weather will use the Manual Weather Settings instead. This is to allow things like animating specific settings in sequencer.^Random Weather VariationAfter applying the current “Weather” variable to global weather, the system applies the random weather. This is 
handled by the Random Weather Variation blueprint component. That BP component selects random weather periodically and lerps from one to the next, keeping it’s random weather in an object called Current Random Weather State. As a part of Update Active Variables on UDW, that random weather state is lerped with the static weather state, using the variable “Lerp to Static Settings”. This only happens if “Use Random Weather Variation” is enabled.^Weather ParticlesUDW has niagara emitter components for Snow, Rain, Dust, and Wind Debris. These are enabled/disabled based on the settings in their resp
ective categories. Each one has a section in Update Static Variables, where their static settings (ones which don’t change with current weather) are set. And they have a function which is run in Update Active Variables too. Each of these functions starts by setting the spawn rate of the emitter, and if that spawn rate is above zero, it sets the rest of the dynamic parameters for the emitter.^LightningLightning Flashes are triggered globally from the Event Tick graph, in the section called “Spawn Global Lightning Flashes”. This runs an event in the Lightning Flash graph which spawns a lightning
 bolt emitter, animates the strength of the lightning directional light, and plays a thunder sound if enabled.^Change WeatherThe Change Weather event exists in the Weather Change Event graph. As does the “Change to Random Weather” function. These events (set to run only on server) set the Transition Mode and reset the timers to start a new transition, either from any current weather to new static type, or from static settings to random settings. You can see where this transition is handled in the Event Tick graph, in the section called Transition Settings.^Sound EffectsThe majority of the logi
c for sound effects is in a function called Audio Update. This is what turns the sounds on and off as necessary and adjusts their volume levels based on the weather state. This also relies on the logic for Sound Occlusion, which is handled in two other functions. Update Target Occlusion Profile, run only periodically, sends out traces to test for occluding objects. And Update Current Occlusion Profile, smoothly lerps from the last test data to the latest set. The Current Occlusion Profile float array is what’s sampled by the sound effects in Audio Update.^Material EffectsMaterial Effects are a
nimated with weather state over time, if the setting “Simulate Changing Material Effects with Weather” is enabled. This logic is located in the function “Update Current Material Effects”. It sets the current snow coverage/wetness based on the local weather state. The actual parameters are then passed to the shared material parameter collection in the function called “Update Material Effect Parameters”.^Dynamic Landscape Weather EffectsThe DLWE effects involve a number of functions, which you can find categorized under the Dynamic Landscape Weather Effects category. The most important include “
Update DLWE Parameters” which sets the shared material parameters used by the DLWE function, based on current weather and the current DLWE mode. The DLWE effects are modal, in that they shift from using their render targets for dynamic snow, over to using them for dynamic puddles, based on the weather. “Update DLWE Snow Compressions” and “Update DLWE Puddle Ripples” are the functions which draw into the render targets, using data delivered by the DLWE Interaction component, when it runs the “Compress DLWE Snow” and “Ripple DLWE Puddle” functions.^Post Process Wind FogThe Post Process Wind Fog 
is a post effect enabled when wind and fog density are high, by default. The effect is started, if enabled, in the function “Set Up Post Process Wind Fog”, and it’s updated in “Update Post Process Wind Fog”. The update function animates the 3D offset of the 3D noise textures in the material, as well as scaling the effect and setting its colors using the current fog color on UDS.^Screen DropletsThe Screen Droplets work similarly to Post Process Wind Fog, with a Start and Update function. The Update function in that case animates the time value for the droplets in the material, and drives the in
tensity up and down based on the camera’s exposure to rain.^Weather Mask BrushesThe Weather Mask Brush system is similar to the DLWE function in that it works by managing a render target, but the structure is a little different. The Weather Mask Brush target is only drawn to as the player moves far enough away from the center of the target area. At that point, it is completely redrawn, using the function “Draw Weather Mask Brush Target”.^TemperatureTemperature is only handled in one place on UDW. The function is called “Get Current Temperature”. It works by deriving a temperature value from th
e current time, weather and season. To clarify, the temperature value is determined by weather, it does not effect weather.^Wind DirectionThere is a simple system of varying the wind direction on UDW. The logic for this is in the function Update Wind Direction. It simply offsets the current direction of wind from the starting direction using a perlin noise function driven by time.
-----
BLUEPRINT - WEATHER OVERRIDE VOLUME
-----
The Weather Override Volume (found in Blueprints/Weather_Effects) is an actor which can apply regional weather to an arbitrary shaped 2D space in a level.
It does this with its editable spline component. The spline represents the area to apply weather to.
The Weather Override Volume also handles telling UDW about itself and removing itself from UDW's list of volumes when it dies/is unloaded.
The local weather state on the weather override volume is stored in a setting object variable called Weather State. It's determined either by a single weather preset copied into it on startup, or by a Random Weather Variation component, the same class as the one that exists on UDW.
On UDW, the function called "Apply Current/Local Weather" runs through the weather volumes and applies them ad needed to the weather state there, by testing against their spline, using the current Control Point Location on UDW (normally determined using the pawn location).
-----
BLUEPRINT - WEATHER MASK BRUSH
-----
The Weather Mask Brush doesn't have much logic of its own. On startup, it will make sure UDW has the weather mask brush target set up, and if it doesn't, call the function that initializes it. And it will remove itself from the UDW list of mask brushes when it's removed/unloaded.
Other than that, the mask brush actor just has logic in it's construction script to turn the settings and location of the brush into variables which UDW can reference to draw the brush into the weather mask target.
-----
BLUEPRINT - RAIN DRIP SPLINE
-----
The Rain Drip Spline BP exists to contain a niagara emitter which follows the curve of a spline.
There are two main functions here. One is "Create Curve Data" which samples the spline and creates the emitter based on it. It also does line traces down for static geometry, which are stored in the emitter as a parameter, to be used as cached collision by the particles.
The other function "Update with Weather" runs periodically and updates the spawn rate of the emitter based on the UDW weather state.
-----
BLUEPRINT - AMBIENT SOUND (TIME AND WEATHER CONTROLLED)
-----
This blueprint (located in Blueprints/Sound) is a child of the standard Ambient Sound class. It contains the additional logic to modulate the volume of the sound with time and weather.
It does this using the event dispatchers that exist on UDS for Sunset/Sunrise, as well as Started Raining/Finished Raining, etc on UDW.
The BP binds to these events as part of its construction script. When one of these events happens, it runs the function "Update Volume Multiplier" to change the new target multiplier based on the situation.
The actor does not tick by default. It has it's tick enabled whenever it needs to change its volume over time. When enabled, the Event Tick sets the volume multiplier of the audio based on the Volume Multiplier Transition Time.
-----
NIAGARA SYSTEMS
-----
^Weather ParticlesThis is the emitter shared by the Rain, Snow, Dust and Wind Debris niagara systems. It contains their shared behavior for collision and movement, based on the parameters set by the UDW blueprint.^Dripping CurveThis is the emitter used by the Rain Drip Spline BP, for emitting drip particles along the spline, using the cached collision values created by the blueprint. Also contains an emitter for spawning splash particles, triggered by the death event of the drips.^DustUses the Weather Particles emitter to spawn dust particles. This is the emitter used on UDW if "Enable Dust Pa
rticles" is turned on.^Inside Cloud FogThis emitter is used on UDS if volumetric fog and "Enable Fog Inside Clouds" are turned on. It uses a single volumetric particle at the camera location to increase volumetric fog density when the camera is inside the volumetric cloud layer.^Lightning StrikeThe main lightning bolt + flash effect, spawned by the Flash Lightning event on UDW. Contains a ribbon particle for the main bolt, plus cards for additional branches and flashes/fake cloud lighting.^Obscured LightningThis emitter is used on UDW to create the idle flashes of hidden lightning, inside the 
clouds. It spawns around the camera location, and its spawn rate is controlled by the Thunder/Lightning level of the current weather state.^Puddle SplashSpawned by the DLWE Interaction component when impacting a collision surface which it detects to have a puddle.^RainUses the Weather Particles emitter to spawn rain particles. Used by UDW if "Enable Rain Particles" is on. Also contains emitters for splash particles and rain spot decals, which are spawned using a death event of the main weather particle emitter.^SnowUses the Weather Particles emitter to spawn snow particles. Used by UDW if "Ena
ble Snow Particles" is on.^Snow TrailSpawned by the DLWE Interaction component while moving against a collision surface it detects to have snow.^SplashesThe emitter used by Rain and Dripping Curve to spawn splashes from raindrops.^Wind DebrisUses the Weather Particles emitter to spawn wind debris (leaves, twigs, whatever is in the debris texture selected on UDW). Used by UDW if "Enable Wind Debris" is on.
-----
MAIN MATERIALS
-----
^Ultra Dynamic Sky MatThe sky sphere material used by Ultra Dynamic Sky. It handles the following:>- The sky background colors, determined either by the simplified color parameters and scattering texture. Or by sampling the sky atmosphere, depending on the Color Mode.
- The cloud wisps, lerped over the background color using the Ambient Color parameter.
- The sun, drawn in the Sun Disk function by sampling the vector parameter Sun Position and comparing it to the Camera Vector.
- The dynamic clouds, if enabled. The clouds are drawn in Composite Cloud Layers function, either one layer or two depending on the settings/platform. The cloud colors are derived from sky atmosphere samples if enabled, and simplified color values if not.
- The Moon. It's handled mainly in the function called Moon. The texture is drawn at the Moon Vector and the channels of the texture are compared against a Moon Phase Angle to render the phase.
- The Stars, handled in the function called Stars. Much of this function is the logic for mapping the real stars texture, for when Simulate Real Stars is enabled.
- 2D Auroras. Not to be confused with the volumetric auroras used when the Sky Mode is set to Volumetric Aurora. These are drawn in the material function called Aurora.
- Control of contrast, saturation and intensity of the sky background.^2D Cloud ShadowsThe light function material applied to the directional lights to create cloud shadows, unless volumetric clouds are enabled, in which case Volumetric Cloud Shadows is used.
The material mimics the texture samples used to make the 2D dynamic clouds and filters those with density the same way, so that the shadows will generally look appropriate for the cloud coverage.^Volumetric Cloud ShadowsThe same principle as the 2D cloud shadows, but tailored to represent the volumetric clouds accurrately. It also has additional logic factoring in the Z height of the pixel being lit, so that the cloud shadows will fade out above the cloud layer.^Cloud Fog Post ProcessUsed to apply a fake fog effect to the camera when inside volumetric clouds, by sampling the same volumetric cl
oud material functions used in the volumetric material.^Inside Clouds Fog ParticleUsed on the inside cloud fog niagara system, to add volumetric fog density when inside volumetric clouds.^Lens FlareThe post process material used to render the sun lens flare, when "Enable Sun Lens Flare" is turned on on UDS. It maps a number of textures using the camera vector combined with the sun vector. The textures and their dimensions are altered in the material instances, one for each lens flare type.^Overcast TurbulenceRendered on a flat plane at the bottom of the volumetric cloud layer, to add additiona
l texture based turbulence to the volumetric clouds when overcast. Also renders fake lighting effects alongside the lightning flashes on UDW.^Static CloudsRenders the static clouds enabled when the Sky Mode on UDS is set to Static Clouds. Samples the two static cloud textures, which contain lighting from 4 directions + up, and an alpha channel. Combines these using lighting state set by UDS in the function "Update Static Clouds". Also fades out opacity as cloud coverage increases, to support weather as well as it can.^Volumetric CloudsThe volumetric material used by the volumetric cloud compon
ent on UDS, when Sky Mode is set to Volumetric Clouds. The volumetric cloud material is evaluated in stages. The first stage is on the left. It uses the function "Volumetric Clouds Conservative Density" to produce the major cloud shapes, completely 2D, like a flat projection of the clouds. Then in the areas which have conservative density above 0, the rest of the material logic is evaluated. The main logic there is in "Volumetric Clouds Extinction" which does the 3D noise texture samples and calculates the tapering/extinction scaling for the clouds, to give them their detail and final look.^Vo
lumetric AuroraUsed on UDS when the Sky Mode is set to Volumetric Aurora. Not to be confused with the 2D aurora effect which can be enabled for the other sky modes.
This volumetric aurora is a much simpler effect than the volumetric clouds. It only samples three moving textures and uses those for conservative density, then filters that some for the final extinction, and lerps 3 color parameters for the emissive color.
-----
WEATHER MATERIALS
-----
^DLWE BrushThe master material for the brushes used by the Dynamic Landscape Weather Effects to draw into the render targets for snow trails and puddle ripples. They just sample a single texture and adjust it's range using a few parameters. These are set in the instances in Materials/Material_Instances/DLWE_Brushes for each use case in the DLWE system.^Dust ParicleMatThe material used for the dust particles. Unlike the snow and rain particle materials, the dust particle mat is unlit, with black as it's constant emissive color. This is to reduce shader instructions as low as they can go, becaus
e of the more intense overdraw requirements of the larger dust particles.^Fog ParticleMatA volumetric material, used by the volumetric fog particle element on the rain, snow and dust emitters. Contains logic for rendering a sphere of volumetric density, filtered with a 3D noise texture sample. Also cuts of density at the location and angle of the particle's collision, to keep the volumetric effect from going through walls.^Lightning Ball ParticleMatA simple radial gradient based particle sprite material, as an element for the lightning flash effect.^Lightning Bolt ParticleMatThe material used 
on the ribbon particle element of the lightning flash niagara system. Samples a texture, and additionally on high material quality, will filter the opacity with a 3D texture sample for added variation.^Lightning Flare ParticleMatThe main flare element used both by lightning flashes and obscured lightning. Contains multiple flickering texture samples and a panning texture sample for the falling rain element.^Post Process Wind FogThe post process material used by the effect of the same name on UDW. Renders additional "fog" using 3D texture samples which move controlled by a vector parameter.  It
 also contains logic for very simple directional occlusion, to disable the effect in each of 4 directions, driven by the sound occlusion values on UDW. This keeps the effect from generally being rendered in a direction that's blocked.^Rain ParticleMatThe material used on the rain particles. Contains a sample of the rain/snow particle texture sheet, and the logic for the "refraction" effect using a scene color sample. The rain particle mat is a lit translucent material, using Volumetric Directional lighting.^Rain Spot DecalA decal material, used by the rain niagara system for leaving little wet
 spots where rain drops fall, if enabled. It reuses the rain and snow particle sprite sheet, using the snow particle sprites for the shape of the effect. The material only outputs roughness in the space it affects.^Screen DropletsThe post process material used by the screen droplets effect on UDW. Samples the drips vertical mask texture, which pans down, and the ScreenDroplets_Small texture, which doesn't pan, to produce a distortion value for a scene texture sample. Also contains logic to multisample that distortion for a blur effect on high quality.^Snow ParticleMatThe material used by the s
now particle system. Functionally very similar to the rain particle mat. Uses the same Volumetric Directional lighting mode.^Snow Trail ParticleA dithered masked material used by the DLWE Interaction component's snow trail particle effect. A dithered masked material is used to avoid lighting problems with large translucent particles intersecting the ground.^Splash Particle MatThe material used by the splash particles in the rain niagara system and the rain drip spline. They sample the splash animation texture (4 frames for looking at the splash from the side, in the red channel, and 4 frames f
or looking from the top, in the green channel.) Note that these are unlit particles and they determine their emissive color using a scattered scene color sample.^Wind DebrisThe material used by the wind debris particles. Samples the texture selected on UDW and fades using the alpha setting, and a camera depth fade. Uses Volumetric Per Vertex Directional lighting.




***
PART 4
***
-----
COMMON ISSUES
-----
This section contains solutions and workarounds for common problems faced when working with UDS and UDW.
-----
THE SKY COLOR FLICKERS BLACK WHEN MOVING THE CAMERA
-----
If you notice the color of the sky flickering to pitch black when the camera is moved in certain places in your level, you're probably encountering an engine bug with the sky atmosphere system.
The issue is caused in some circumstances by the camera going below the "ground" defined by the sky atmosphere component.
So the best solution is to select the Sky Atmosphere component in UDS (found in the component list at the top of the details panel) and move it down to the lowest point in your level, below where the camera can go.
-----
LIGHTING ARTIFACTS IN CLOUDY WEATHER USING HARDWARE RAYTRACING
-----
The hardware raytraced reflections and global illumination cannot factor in the light function used to occlude the sun and moon during cloudy weather. As a result, with default settings, cloudy weather produces artifacts in reflections and indrect light.
There's a setting which can fix this on UDS. In Basic Controls, turn on "Dim Directional Lights with Cloud Coverage". This turns the actual intensity of the sun and moon down to 0 as clouds form, so that ray traced effects can work generally as expected.
-----
BROKEN WEATHER PARTICLES WITH RAYTRACING
-----
If you have Raytraced Translucency enabled, weather particles like rain, snow and lightning will be broken.
This is just a result of Raytraced Translucency being incompatible with translucent sprite bilboard based particles. The only fix is to disable Raytraced Translucency. You can do this on a post process volume. In the Tranlucency section, change Type to Raster.
-----
AMBIENT LIGHT NOT UPDATING AS TIME OF DAY CHANGES
-----
If you notice that the ambient light starts out looking correct, but as time of day changes, the ambient light doesn't update, it's almost certainly a problem with the sky light not updating. Here are a few possible causes of that:>If you see this problem on specific platforms, it's probably because they don't support real time sky light capture. I'd recommend changing Sky Light Mode to Cubemap with Dynamic Color Tinting for a good dynamic option that works on any platform.
If you specifically notice ambient light remaining bright as night falls, you could also just have static light from an old lighting solution present in your map. To check for that, just run a lighting build, from the Build dropdown on the main editor toolbar.
-----
WEATHER PARTICLES NOT SHOWING IN SIMULATE MODE
-----
If running in editor using the "Simulate" play mode, weather particles will probably not be visible.
This is just a limitation of Simulate mode and the niagara interface the particles use to access camera data. In Simulate, the particles can't get the camera location, so they only spawn around the origin.
Since this is an engine limitation, there's no workaround at this time.
-----
SNOW/DUST COVERAGE AND WETNESS IN INTERIORS
-----
After adding snow and wetness material effects to your materials, you might be disappointed to find that without additional work they show up in interiors.
This is normal. The material functions pixel and vertex shader logic have no way of knowing what context they're used in. They don't know if the surface is "inside" or not. And the best way for a material to sample that information will vary from project to project and from material to material.
One option is to mask the effects using the inputs on the material functions. "Mask Snow/Dust Coverage" on Snow Weather Effects, "Mask Wetness" on Wet Weather Effects, and "Mask Snow/Dust Coverage" and "Mask Wetness" on the DLWE function. These take in a 0 to 1 mask which completely blocks formation at a value of 0.
A simple and cheap method of controlling this mask in a static way is to use a vertex color channel. You could plug a channel of vertex color into Max Snow/Dust Coverage and paint the mesh in the level editor to define what parts form snow, for example.
Another option if you need isolated specific control of masking is to use the Weather Mask Brush actor, in Blueprints/Weather_Effects. It can mask an area using simple brush shapes projected on the Z axis. For example, if you needed the space under a large tree to gather less snow, you could place a weather mask brush which draws a soft radial falloff on the ground around it.
-----
DLWE INTERACTIONS NOT HAPPENING WHEN THEY SHOULD
-----
If you've set up DLWE (following the directions laid out in the DLWE section of this readme) and interactions aren't happening, here's a list of things to check:>• Make sure your landscape (or static mesh) has simple collision using the Object Type specified by "Landscape Object Type" in the DLWE settings on UDW. By default, this is World Static.
• Make certain "Enable Dynamic Landscape Weather Effects" is on.
• Make sure the DLWE Interaction component is positioned and has a size big enough to trigger interactions with the surface.
-----
VOLUMETRIC CLOUDS DON'T RENDER IN FRONT OF OBJECTS
-----
If using the default settings, you might notice clouds appearing incorrectly behind scenery that they should block.
By default, the volumetric clouds in UDS are optimized for best fidelity in typical use cases where the clouds exist entirely as a background layer. This allows them to prioritize fidelity and temporal stability.
If you need your clouds to render over the landscape/objects in your scene, the fix is simple. In the Volumetric Clouds category on UDS, just change the "Volumetric Cloud Rendering Mode" to one of the settings called "Clouds Render Over Opaque Objects".
-----
WEATHER PARTICLES NOT COLLIDING WITH OBJECTS
-----
If you're having trouble with weather  particles not being blocked by objects that they seemingly should, here are a few things to check:>• Make sure the object has simple collision. The traces used by the particles only check for simple collision.
• Make sure the collision profile of the object is set to block the trace channel selected on UDW as "Weather Particle Collision Channel". By default this is Visibility. Also make sure the collision response is Collision Enabled or Query Only, as queries are what the particles use.
• If particles are coming through the roof of a building with a very tall ceiling, it may be that the particles aren't checking high enough for the ceiling when they spawn in. You can adjust this distance using "Ceiling Check Height" on UDW.
-----
LIGHT ARTIFACTS WHEN INSIDE VOLUMETRIC CLOUDS
-----
When going up into the volumetric cloud layer you might notice banding/noise or other light artifacts in the volumetric rendering while in the layer.
The settings are optimized to prioritize quality from the ground, so this is normal for default settings. Here are a few settings to try adjusting to improve render quality within the layer:>• Increase "View Sample Count Scale" This scales the number of samples used in the volumetric rendering. Increasing it will reduce noise in the resolve and can fix some artifacts if pushed high enough. Note that this will increase the performance demand of the clouds proportional to the amount you increase the setting.
• Increase "Shadow Sample Scale". This will reduce banding of shadows and make them have a smoother falloff. Again, comes with extra performance cost.
• Increase "Distance to Sample Max Count" in the advanced dropdown. This can help in particular with the horizontal band of light that can be seen from within the cloud layer.
• Reduce "Tracing Max Distance". There are two settings for this, one for inside the cloud layer, and one for outside. Because when in the cloud layer, it makes sense for this value to be lower, to reduce the amount of the layer the traces will try to sample.
-----
BLACK NOISE ARTIFACTS WITH YELLOW WARNING TEXT ON MESHES
-----
If you're seeing flickering black pixel noise on certain materials, possibly with a yellow warning message visible in the black space, the problem is probably a bad implementation of pixel depth offset on the material.
The black space + yellow warning text means that the pixel is rendering nothing due to the material being broken for that pixel. This can happen due to pixel depth offset, so the first thing I would recommend is to disconnect the Pixel Depth Offset output from the material, if it has one.
Other causes for this can sometimes be LOD meshes with "Dither LOD Transition" enabled, or materials with "Dither Opacity Mask" enabled.
-----
EVERYTHING RENDERING BLACK AFTER ADDING UDS
-----
If after adding UDS to your scene, the entire level renders black, the cause is almost certainly exposure settings. Here's a few things to try:>• In your scene, check to see if you have any post process volumes which are controlling the exposure settings. If you do, disable those overrides so UDS can control exposure.
• On UDS, try disabling "Use Exposure Range" in the exposure category, or adjusting the min and max.
-----
VOLUMETRIC CLOUDS LOOK BLURRY/STREAKY WHEN MOVING FAST
-----
By default, the volumetric clouds use a rendering mode which uses a long tail of temporal data. Because of this, when the clouds move fast, like during a time lapse, the rendering can become blurry or ghosted.
The solution is to use any of the other rendering modes. You can change the Volumetric Cloud Rendering Mode from the Volumetric Clouds category on UDS.
-----
WEATHER SOUNDS GO QUIET AT INCORRECT TIMES
-----
By default, the weather sounds on UDW use a simple system of sound occlusion to dim and muffle weather when inside interiors.
If you're noticing this happen at incorrect times, it's probably because the occlusion traces are hitting surfaces they shouldn't. You can adjust the behavior of the occlusion system (or disable it completely) from the Sound Occlusion category on UDW.
-----
WATER/OCEAN SHADER LIGHTS INCORRECTLY IN CLOUDY WEATHER
-----
If using some types of water or ocean shaders, you might notice in cloudy weather that the sun or moon appear to affect the material despite being blocked by clouds. This would be because some materials like this test the intensity and color of the active directional light without factoring in light functions.
The solution is to enable a setting on UDS, in the Basic Controls category, called "Dim Directional Lights With Cloud Coverage". This will reduce directional light intensity when cloudy so that most water shaders like this will be able to function correctly.
-----
VOLUMETRIC CLOUDS ARE TOO DARK / GRAY
-----
If the volumetric clouds look unnatrually dark, the problem is probably the directional light affecting them.
Often, this will happen if you've selected "custom actor" for the sun or moon. The built in sun and moon lights have a particular setting customized which you would need to apply to your own light to have them light volumetric clouds well. The setting is "Cloud Scattered Luminance Scale". For best results I would suggest copying the exact color from UDS' directional light components.
If you haven't selected "custom actor", the problem may instead be that you have a directional light in your scene that you've forgotten to delete before adding UDS.
-----
CANNOT ADJUST CLOUD SPEED OR DIRECTION ON UDS
-----
If you're trying to adjust Cloud Speed or Cloud Direction on UDS and cannot, that would be because you have UDW in your scene as well.
If you have UDW in your scene, cloud movement is controlled by the current wind, which is an aspect of the weather. Which is why they aren't controllable directly on UDS.
To change these with UDW in your scene, adjust the variables "Cloud Speed Multiplier" and "Wind Direction" on UDW.