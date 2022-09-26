#include <Arduino.h>

static const char animation_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
    <style>
        .page-grid {
            display: grid;
            grid-template-columns: auto 400px auto;
        }

        .channel-grid {
            display: grid;
            gap: 5px;
            grid-template-columns: 100px 100px 100px 100px;
            width: 400px;
            padding: 5px;
        }

        .channel-grid-item {
            background-color: rgba(255, 255, 255, 0.8);
            border: 1px solid rgba(0, 0, 0, 0.8);
            padding: 0px;
            font-size: 20px;
            text-align: center;
        }

        .channel-button {
            width: 100%;
            height: 100%;
        }

        .animation-grid {
            display: grid;
            gap: 5px;
            grid-template-columns: 100px 100px;
            width: 200px;
            margin: auto;
        }

        .color-palette-grid {
            display: grid;
            gap: 5px;
            grid-template-columns: 100px 100px 100px;
            width: 300px;
            padding: 0px;
            margin: auto;
        }

        .left-column {
            text-align: right;
        }

        .middle-column {
            text-align: center;
        }

        .right-column {
            text-align: left;
        }

    </style>
</head>

<body>
    <div class="page-grid">
        <div class="left-column"></div>
        <div class="middle-column">
            <h1>Light Config Home</h1>
            <div>
                <H2>Animation</H2>
                <form action="/animation" method="post">
                    <div class="animation-grid">
                        <label for="animation" class="left-column">Animation:</label>
                        <select name="animation" id="animation" class="right-column">
                            <option value="0" %SOLID_ANIMATION_SELECTED%>Solid</option>
                            <option value="1" %SWEEPFRONT_ANIMATION_SELECTED%>Sweep Front</option>
                            <option value="2" %SWEEPREAR_ANIMATION_SELECTED%>Sweep Rear</option>
                            <option value="3" %SWEEPRIGHT_ANIMATION_SELECTED%>Sweep Right</option>
                            <option value="4" %SWEEPLEFT_ANIMATION_SELECTED%>Sweep Left</option>
                            <option value="5" %SWEEPUP_ANIMATION_SELECTED%>Sweep Up</option>
                            <option value="6" %SWEEPDOWN_ANIMATION_SELECTED%>Sweep Down</option>
                            <option value="7" %SPINCLOCK_ANIMATION_SELECTED%>Spin Clockwise</option>
                        </select>
                        
                        <label for="fps" class="left-column">FPS:</label>
                        <input type="range" id="fps" name="fps" value=%REPLACE_ANIMATION_FPS% min="1" max="30" step="1"
                            class="right-column">

                        <label for="stepSize" class="left-column">Step Size:</label>
                        <input type="range" id="stepSize" name="stepSize" value=%REPLACE_ANIMATION_STEPSIZE% min="1"
                            max="50" step="1" class="right-column">

                        <label for="brightness" class="left-column">Brightness:</label>
                        <input type="range" id="brightness" name="brightness" value=%REPLACE_ANIMATION_BRIGHTNESS%
                            min="1" max="255" step="1" class="right-column">

                        <label for="blending" class="left-column">Blend Colors: </label>
                        <input type="checkbox" id="blending" name="blending" value=true %REPLACE_ANIMATION_BLEND%
                            class="right-column">

                        <label for="colorPalette" class="left-column">Color Palette:</label>
                        <select name="colorPalette" id="colorPalette" class="right-column">
                            <option value="0" %CUSTOM_PALETTE_SELECTED%>Custom</option>
                            <option value="1" %RAINBOW_PALETTE_SELECTED%>Rainbow</option>
                            <option value="2" %RAINBOWSTRIPE_PALETTE_SELECTED%>Rainbow Stripes</option>
                            <option value="3" %PARTY_PALETTE_SELECTED%>Party</option>
                            <option value="4" %LAVA_PALETTE_SELECTED%>Lava</option>
                            <option value="5" %HEAT_PALETTE_SELECTED%>Heat</option>
                            <option value="6" %CLOUD_PALETTE_SELECTED%>Cloud</option>
                            <option value="7" %OCEAN_PALETTE_SELECTED%>Ocean</option>
                            <option value="8" %FOREST_PALETTE_SELECTED%>Forest</option>
                        </select>
                    </div>
                    <h3>Custom Color Palette</h3>
                    <div class="color-palette-grid">
                        <label>Index</label>
                        <label class="middle-column">Palette Position</label>
                        <label>Color</label>
                        <label for="color0Pos" class="left-column">Color 0:</label>
                        <input type="number" id="color0Pos" name="color0Pos" value=%COLOR_0_POS% min="0" max="255"
                            class="middle-column">
                        <input type="color" id="color0" name="color0" value=%COLOR_0_HEX% class="right-column">

                        <label for="color1Pos" class="left-column">Color 1:</label>
                        <input type="number" id="color1Pos" name="color1Pos" value=%COLOR_1_POS% min="0" max="255"
                            class="middle-column">
                        <input type="color" id="color1" name="color1" value=%COLOR_1_HEX% class="right-column">

                        <label for="color2Pos" class="left-column">Color 2:</label>
                        <input type="number" id="color2Pos" name="color2Pos" value=%COLOR_2_POS% min="0" max="255"
                            class="middle-column">
                        <input type="color" id="color2" name="color2" value=%COLOR_2_HEX% class="right-column">

                        <label for="color3Pos" class="left-column">Color 3:</label>
                        <input type="number" id="color3Pos" name="color3Pos" value=%COLOR_3_POS% min="0" max="255"
                            class="middle-column">
                        <input type="color" id="color3" name="color3" value=%COLOR_3_HEX% class="right-column">

                        <label for="color4Pos" class="left-column">Color 4:</label>
                        <input type="number" id="color4Pos" name="color4Pos" value=%COLOR_4_POS% min="0" max="255"
                            class="middle-column">
                        <input type="color" id="color4" name="color4" value=%COLOR_4_HEX% class="right-column">

                        <label for="color5Pos" class="left-column">Color 5:</label>
                        <input type="number" id="color5Pos" name="color5Pos" value=%COLOR_5_POS% min="0" max="255"
                            class="middle-column">
                        <input type="color" id="color5" name="color5" value=%COLOR_5_HEX% class="right-column">

                        <label for="color6Pos" class="left-column">Color 6:</label>
                        <input type="number" id="color6Pos" name="color6Pos" value=%COLOR_6_POS% min="0" max="255"
                            class="middle-column">
                        <input type="color" id="color6" name="color6" value=%COLOR_6_HEX% class="right-column">

                        <label for="color7Pos" class="left-column">Color 7:</label>
                        <input type="number" id="color7Pos" name="color7Pos" value=%COLOR_7_POS% min="0" max="255"
                            class="middle-column">
                        <input type="color" id="color7" name="color7" value=%COLOR_7_HEX% class="right-column">

                        <label for="color8Pos" class="left-column">Color 8:</label>
                        <input type="number" id="color8Pos" name="color8Pos" value=%COLOR_8_POS% min="0" max="255"
                            class="middle-column">
                        <input type="color" id="color8" name="color8" value=%COLOR_8_HEX% class="right-column">

                        <label for="color9Pos" class="left-column">Color 9:</label>
                        <input type="number" id="color9Pos" name="color9Pos" value=%COLOR_9_POS% min="0" max="255"
                            class="middle-column">
                        <input type="color" id="color9" name="color9" value=%COLOR_9_HEX% class="right-column">

                    </div>
                    <input type="submit" value="Update">
                </form>
                <div>
                    <H2>Edit Channel Details</H2>
                    <div class="channel-grid">
                        <div calss="channel-grid-item">
                            <form action=" /channels" method="post" class="options-grid">
                                <input type="hidden" name="CHANNEL_INDEX" value="0" />
                                <input type="hidden" name="INITIAL" value="true" />
                                <input type="submit" value="Channel 0" class="channel-button">
                            </form>
                        </div>
                        <div calss="channel-grid-item">
                            <form action=" /channels" method="post" class="options-grid">
                                <input type="hidden" name="CHANNEL_INDEX" value="1" />
                                <input type="hidden" name="INITIAL" value="true" />
                                <input type="submit" value="Channel 1" class="channel-button">
                            </form>
                        </div>
                        <div calss="channel-grid-item">
                            <form action=" /channels" method="post" class="options-grid">
                                <input type="hidden" name="CHANNEL_INDEX" value="2" />
                                <input type="hidden" name="INITIAL" value="true" />
                                <input type="submit" value="Channel 2" class="channel-button">
                            </form>
                        </div>
                        <div calss="channel-grid-item">
                            <form action=" /channels" method="post" class="options-grid">
                                <input type="hidden" name="CHANNEL_INDEX" value="3" />
                                <input type="hidden" name="INITIAL" value="true" />
                                <input type="submit" value="Channel 3" class="channel-button">
                            </form>
                        </div>
                        <div calss="channel-grid-item">
                            <form action=" /channels" method="post" class="options-grid">
                                <input type="hidden" name="CHANNEL_INDEX" value="4" />
                                <input type="hidden" name="INITIAL" value="true" />
                                <input type="submit" value="Channel 4" class="channel-button">
                            </form>
                        </div>
                        <div calss="channel-grid-item">
                            <form action=" /channels" method="post" class="options-grid">
                                <input type="hidden" name="CHANNEL_INDEX" value="5" />
                                <input type="hidden" name="INITIAL" value="true" />
                                <input type="submit" value="Channel 5" class="channel-button">
                            </form>
                        </div>
                        <div calss="channel-grid-item">
                            <form action=" /channels" method="post" class="options-grid">
                                <input type="hidden" name="CHANNEL_INDEX" value="6" />
                                <input type="hidden" name="INITIAL" value="true" />
                                <input type="submit" value="Channel 6" class="channel-button">
                            </form>
                        </div>
                        <div calss="channel-grid-item">
                            <form action=" /channels" method="post" class="options-grid">
                                <input type="hidden" name="CHANNEL_INDEX" value="7" />
                                <input type="hidden" name="INITIAL" value="true" />
                                <input type="submit" value="Channel 7" class="channel-button">
                            </form>
                        </div>
                        <div calss="channel-grid-item">
                            <form action=" /channels" method="post" class="options-grid">
                                <input type="hidden" name="CHANNEL_INDEX" value="8" />
                                <input type="hidden" name="INITIAL" value="true" />
                                <input type="submit" value="Channel 8" class="channel-button">
                            </form>
                        </div>
                        <div calss="channel-grid-item">
                            <form action=" /channels" method="post" class="options-grid">
                                <input type="hidden" name="CHANNEL_INDEX" value="9" />
                                <input type="hidden" name="INITIAL" value="true" />
                                <input type="submit" value="Channel 9" class="channel-button">
                            </form>
                        </div>
                        <div calss="channel-grid-item">
                            <form action=" /channels" method="post" class="options-grid">
                                <input type="hidden" name="CHANNEL_INDEX" value="10" />
                                <input type="hidden" name="INITIAL" value="true" />
                                <input type="submit" value="Channel 10" class="channel-button">
                            </form>
                        </div>
                        <div calss="channel-grid-item">
                            <form action=" /channels" method="post" class="options-grid">
                                <input type="hidden" name="CHANNEL_INDEX" value="11" />
                                <input type="hidden" name="INITIAL" value="true" />
                                <input type="submit" value="Channel 11" class="channel-button">
                            </form>
                        </div>
                        <div calss="channel-grid-item">
                            <form action=" /channels" method="post" class="options-grid">
                                <input type="hidden" name="CHANNEL_INDEX" value="12" />
                                <input type="hidden" name="INITIAL" value="true" />
                                <input type="submit" value="Channel 12" class="channel-button">
                            </form>
                        </div>
                        <div calss="channel-grid-item">
                            <form action=" /channels" method="post" class="options-grid">
                                <input type="hidden" name="CHANNEL_INDEX" value="13" />
                                <input type="hidden" name="INITIAL" value="true" />
                                <input type="submit" value="Channel 13" class="channel-button">
                            </form>
                        </div>
                        <div calss="channel-grid-item">
                            <form action=" /channels" method="post" class="options-grid">
                                <input type="hidden" name="CHANNEL_INDEX" value="14" />
                                <input type="hidden" name="INITIAL" value="true" />
                                <input type="submit" value="Channel 14" class="channel-button">
                            </form>
                        </div>
                        <div calss="channel-grid-item">
                            <form action=" /channels" method="post" class="options-grid">
                                <input type="hidden" name="CHANNEL_INDEX" value="15" />
                                <input type="hidden" name="INITIAL" value="true" />
                                <input type="submit" value="Channel 15" class="channel-button">
                            </form>
                        </div>
                    </div>
                </div>
                <div>
                    <form action="/view" method="get" class="options-grid">
                        <input type="submit" value="View Grid">
                    </form>
                    <form action="/switches" method="get" class="options-grid">
                        <input type="submit" value="Switch Panel">
                    </form>
                </div>
            </div>
        </div>
        <div class="right-column"></div>
    </div>
</body>
</html>
%LAST_LINE%)rawliteral";