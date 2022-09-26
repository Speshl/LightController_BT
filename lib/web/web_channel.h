#include <Arduino.h>

static const char channel_html[] PROGMEM = R"rawliteral(
    <!DOCTYPE html>
<html>

<head>
    <style>
        .page-grid {
            display: grid;
            grid-template-columns: auto 400px auto;
        }

        .options-grid {
            display: grid;
            grid-template-columns: 100px 100px;
            width: 200px;
            margin: auto;
            gap: 5px;
            padding-top: 10px;
            padding-bottom: 10px;
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
        <div></div>
        <div class="middle-column">
            <H2>Channel %CHANNEL_INDEX% Editor</H2>
            <form action="/" method="">
                <input type="submit" value="Home">
            </form>
            <form action=" /channels" method="post" class="options-grid">
                <input type="hidden" name="CHANNEL_INDEX" value="%CHANNEL_INDEX%" />
                <label for="CHANNEL_enabled" class="left-column">Enabled: </label>
                <input type="checkbox" id="CHANNEL_enabled" name="CHANNEL_enabled" value=true %CHANNEL_ENABLED%>

                <label for="CHANNEL_interior" class="left-column" >Interior: </label>
                <input type="checkbox" id="CHANNEL_interior" name="CHANNEL_interior" value=true
                    %CHANNEL_INTERIOR%>
                <!-- 
        <label for="CHANNEL_directionFlipped">DirectionFlipped: </label>
        <input type="checkbox" id="CHANNEL_enabled" name="CHANNEL_enabled" value=true %CHANNEL_DIRECTIONFLIPPED%>
        -->
                <label for="CHANNEL_leftTurn" class="left-column">LeftTurn:  </label>
                <input type="checkbox" id="CHANNEL_leftTurn" name="CHANNEL_leftTurn" value=true
                    %CHANNEL_LEFTTURN%>

                <label for="CHANNEL_rightTurn" class="left-column">RightTurn: </label>
                <input type="checkbox" id="CHANNEL_rightTurn" name="CHANNEL_rightTurn" value=true
                    %CHANNEL_RIGHTTURN%>

                <label for="CHANNEL_brake" class="left-column">Brake: </label>
                <input type="checkbox" id="CHANNEL_brake" name="CHANNEL_brake" value=true %CHANNEL_BRAKE%>

                <label for="CHANNEL_reverse" class="left-column">Reverse: </label>
                <input type="checkbox" id="CHANNEL_reverse" name="CHANNEL_reverse" value=true %CHANNEL_REVERSE%>

                <label for="CHANNEL_type" class="left-column">Type: </label>
                <select name="CHANNEL_type" id="CHANNEL_type">
                    <option value="0" %CHANNEL_TYPE0_SELECTED%>WS2811</option>
                    <option value="1" %CHANNEL_TYPE1_SELECTED%>WS2812</option>
                </select>

                <label for="CHANNEL_order" class="left-column">Order: </label>
                <select name="CHANNEL_order" id="CHANNEL_order">
                    <option value="0" %CHANNEL_RGB_SELECTED%>RGB</option>
                    <option value="1" %CHANNEL_RBG_SELECTED%>RBG</option>
                    <option value="2" %CHANNEL_BRG_SELECTED%>BRG</option>
                    <option value="3" %CHANNEL_BGR_SELECTED%>BGR</option>
                    <option value="4" %CHANNEL_GBR_SELECTED%>GBR</option>
                    <option value="5" %CHANNEL_GRB_SELECTED%>GRB</option>
                </select>

                <label for="CHANNEL_numLEDs" class="left-column"># LEDs: </label>
                <input type="number" id="CHANNEL_numLEDs" name="CHANNEL_numLEDs" value=%CHANNEL_NUMLEDS% min="0"
                    max="100">
                <div></div>
                <input type="submit" value="Update">
            </form>
            <form action="/channelsPos" method="post">
                <h3>Place Pixels on Grid</h3>
                <input type="hidden" name="CHANNEL_INDEX" value="%CHANNEL_INDEX%" />
                <input type="hidden" name="INITIAL" value="true" />
                <input type="submit" value="Place Pixels"><br>
                <label>*Note: # Leds should be entered and updated before attempting to place pixels</label>
            </form>
        </div>
        <div></div>
    </div>
</body>

</html>
%LAST_LINE%)rawliteral";