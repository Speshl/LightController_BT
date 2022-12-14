#include <Arduino.h>

static const char channelLoc_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
    <style>
        
        div.sticky {
            position: -webkit-sticky; /* Safari */
            position: sticky;
            top: 0;
            background-color: white;
        }

        .page-grid {
            display: grid;
            grid-template-columns: auto 350px auto;
        }

        .padded {
            margin-bottom: 5px;
        }

        .tool-grid {
            display: grid;
            grid-template-columns: 80px 80px 80px 80px;
            gap: 5px;
            padding-bottom: 5px;
            margin: auto;
            text-align: center;
        }

        .pos-grid {
            display: grid;
            grid-template-columns: 20px 100px 100px 100px;
            gap: 5px;
            padding-bottom: 5px;
            margin: auto;
            text-align: center;
        }

        .left-column {
            text-align: right;
        }

        .left-column_fill {
            text-align: right;
        }

        .middle-column {
            text-align: center;
        }

        .right-column {
            text-align: left;
        }
    </style>
    <script>
        let lastFocus = "";

        function processLastFocused(min, max, shouldPrompt) {
            if(lastFocus == "" || lastFocus == null){
                console.log("No lastFocus Found");
                return;
            }
            let focusedElement= document.getElementById(lastFocus);
            let value = parseInt(focusedElement.value);
            if(isNaN(value) == true || (shouldPrompt && (value.length == 0 || parseInt(value) < min || parseInt(value) > max))){
                value = parseInt(prompt("Enter value between ("+min+" - "+max+")", "0"));
            }

            let splitID = lastFocus.split("_");
            let type = "_ROW";
            if(splitID.length >= 2){
                id = splitID[1];
                type = splitID[2];
            }
            return {"value":value, "id": id, "type":type};
        }

        function updateFocusedElement(id){
            if(id.includes("_ROW") || id.includes("_COL") || id.includes("_HEIGHT")){
                lastFocus = id;
            }
        }

        function fillTool(numPos, min, max) {
            processedLastFocused = processLastFocused(min,max,true);
            if(processedLastFocused == null){
                return;
            }

            let focusedValue = document.getElementById(lastFocus);
            for(let i=0; i<=99; i++){
                let nextID = "LOC_"+i+"_"+processedLastFocused.type;
                let next = document.getElementById(nextID);
                next.value = processedLastFocused.value;
            }
        }

        function centerTool(numPos, min, max) {
            let midPoint = ((max-min)+1) / 2;
            let posMidPoint = numPos / 2;
            let start = Math.round(midPoint - posMidPoint);

            processedLastFocused = processLastFocused(min,max,false);
            if(processedLastFocused == null){
                return;
            }
            for(let i=0; i<=99; i++){
                let nextID = "LOC_"+i+"_"+processedLastFocused.type;
                let next = document.getElementById(nextID);
                next.value = start + i;
            }
        }

        function cascadeDownTool(numPos, min, max) {
            processedLastFocused = processLastFocused(min,max,true);
            if(processedLastFocused == null){
                return;
            }
            let offset = 0;
            for(let i=processedLastFocused.id; i<=99; i++){
                let nextID = "LOC_"+i+"_"+processedLastFocused.type;
                let next = document.getElementById(nextID);
                next.value = processedLastFocused.value + offset;
                offset++;
            }
        }

        function cascadeUpTool(numPos, min, max) {
            processedLastFocused = processLastFocused(min,max,true);
            if(processedLastFocused == null){
                return;
            }
            let offset = 0;
            for(let i=processedLastFocused.id; i>=0; i--){
                let nextID = "LOC_"+i+"_"+processedLastFocused.type;
                let next = document.getElementById(nextID);
                next.value = processedLastFocused.value + offset;
                offset++;
            }
        }
    </script>
</head>

<body>
    <div class="page-grid">
        <div class="left-column">
            <input type="button" value="Back" onclick="history.back()">
        </div>
        <div class="middle-column">
            <form action="/" method="">
                <input type="submit" value="Home">
            </form>
            <H2>Channel %CHANNEL_INDEX%</H2>
            <h3>Place Pixels on Grid</h3>
            <form action="/channelsPos" method="post">
                <input type="hidden" name="CHANNEL_INDEX" value="%CHANNEL_INDEX%">
                
                <div class="tool-grid sticky">
                    <button type="button" id="fill" onclick="fillTool(%NUM_POS%,0,254)">Fill</button>
                    <button type="button" id="cascade" onclick="cascadeUpTool(%NUM_POS%,0,254)">Cascade &#8593;</button>
                    <button type="button" id="cascade" onclick="cascadeDownTool(%NUM_POS%,0,254)">Cascade &#8595;</button>
                    <button type="button" id="center" onclick="centerTool(%NUM_POS%,0,254)">Center</button>
                </div>
                <div class="pos-grid">
                    <label>Idx</label>
                    <label>Row</label>
                    <label>Col</label>
                    <label>Height</label>
                </div>
                <div class="pos-grid" %LOC_0_HIDDEN%>
                    <label for="LOC_0_POS" class="left-column_fill">0:</label>
                    <input type="number" id="LOC_0_ROW" name="LOC_0_ROW" value="%LOC_0_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_0_COL" name="LOC_0_COL" value="%LOC_0_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_0_HEIGHT" name="LOC_0_HEIGHT" value="%LOC_0_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_1_HIDDEN%>
                    <label for="LOC_1_POS" class="left-column_fill">1:</label>
                    <input type="number" id="LOC_1_ROW" name="LOC_1_ROW" value="%LOC_1_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_1_COL" name="LOC_1_COL" value="%LOC_1_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_1_HEIGHT" name="LOC_1_HEIGHT" value="%LOC_1_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_2_HIDDEN%>
                    <label for="LOC_2_POS" class="left-column_fill">2:</label>
                    <input type="number" id="LOC_2_ROW" name="LOC_2_ROW" value="%LOC_2_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_2_COL" name="LOC_2_COL" value="%LOC_2_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_2_HEIGHT" name="LOC_2_HEIGHT" value="%LOC_2_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_3_HIDDEN%>
                    <label for="LOC_3_POS" class="left-column_fill">3:</label>
                    <input type="number" id="LOC_3_ROW" name="LOC_3_ROW" value="%LOC_3_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_3_COL" name="LOC_3_COL" value="%LOC_3_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_3_HEIGHT" name="LOC_3_HEIGHT" value="%LOC_3_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_4_HIDDEN%>
                    <label for="LOC_4_POS" class="left-column_fill">4:</label>
                    <input type="number" id="LOC_4_ROW" name="LOC_4_ROW" value="%LOC_4_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_4_COL" name="LOC_4_COL" value="%LOC_4_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_4_HEIGHT" name="LOC_4_HEIGHT" value="%LOC_4_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_5_HIDDEN%>
                    <label for="LOC_5_POS" class="left-column_fill">5:</label>
                    <input type="number" id="LOC_5_ROW" name="LOC_5_ROW" value="%LOC_5_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_5_COL" name="LOC_5_COL" value="%LOC_5_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_5_HEIGHT" name="LOC_5_HEIGHT" value="%LOC_5_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_6_HIDDEN%>
                    <label for="LOC_6_POS" class="left-column_fill">6:</label>
                    <input type="number" id="LOC_6_ROW" name="LOC_6_ROW" value="%LOC_6_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_6_COL" name="LOC_6_COL" value="%LOC_6_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_6_HEIGHT" name="LOC_6_HEIGHT" value="%LOC_6_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_7_HIDDEN%>
                    <label for="LOC_7_POS" class="left-column_fill">7:</label>
                    <input type="number" id="LOC_7_ROW" name="LOC_7_ROW" value="%LOC_7_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_7_COL" name="LOC_7_COL" value="%LOC_7_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_7_HEIGHT" name="LOC_7_HEIGHT" value="%LOC_7_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_8_HIDDEN%>
                    <label for="LOC_8_POS" class="left-column_fill">8:</label>
                    <input type="number" id="LOC_8_ROW" name="LOC_8_ROW" value="%LOC_8_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_8_COL" name="LOC_8_COL" value="%LOC_8_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_8_HEIGHT" name="LOC_8_HEIGHT" value="%LOC_8_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_9_HIDDEN%>
                    <label for="LOC_9_POS" class="left-column_fill">9:</label>
                    <input type="number" id="LOC_9_ROW" name="LOC_9_ROW" value="%LOC_9_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_9_COL" name="LOC_9_COL" value="%LOC_9_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_9_HEIGHT" name="LOC_9_HEIGHT" value="%LOC_9_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_10_HIDDEN%>
                    <label for="LOC_10_POS" class="left-column_fill">10:</label>
                    <input type="number" id="LOC_10_ROW" name="LOC_10_ROW" value="%LOC_10_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_10_COL" name="LOC_10_COL" value="%LOC_10_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_10_HEIGHT" name="LOC_10_HEIGHT" value="%LOC_10_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_11_HIDDEN%>
                    <label for="LOC_11_POS" class="left-column_fill">11:</label>
                    <input type="number" id="LOC_11_ROW" name="LOC_11_ROW" value="%LOC_11_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_11_COL" name="LOC_11_COL" value="%LOC_11_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_11_HEIGHT" name="LOC_11_HEIGHT" value="%LOC_11_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_12_HIDDEN%>
                    <label for="LOC_12_POS" class="left-column_fill">12:</label>
                    <input type="number" id="LOC_12_ROW" name="LOC_12_ROW" value="%LOC_12_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_12_COL" name="LOC_12_COL" value="%LOC_12_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_12_HEIGHT" name="LOC_12_HEIGHT" value="%LOC_12_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_13_HIDDEN%>
                    <label for="LOC_13_POS" class="left-column_fill">13:</label>
                    <input type="number" id="LOC_13_ROW" name="LOC_13_ROW" value="%LOC_13_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_13_COL" name="LOC_13_COL" value="%LOC_13_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_13_HEIGHT" name="LOC_13_HEIGHT" value="%LOC_13_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_14_HIDDEN%>
                    <label for="LOC_14_POS" class="left-column_fill">14:</label>
                    <input type="number" id="LOC_14_ROW" name="LOC_14_ROW" value="%LOC_14_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_14_COL" name="LOC_14_COL" value="%LOC_14_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_14_HEIGHT" name="LOC_14_HEIGHT" value="%LOC_14_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_15_HIDDEN%>
                    <label for="LOC_15_POS" class="left-column_fill">15:</label>
                    <input type="number" id="LOC_15_ROW" name="LOC_15_ROW" value="%LOC_15_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_15_COL" name="LOC_15_COL" value="%LOC_15_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_15_HEIGHT" name="LOC_15_HEIGHT" value="%LOC_15_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_16_HIDDEN%>
                    <label for="LOC_16_POS" class="left-column_fill">16:</label>
                    <input type="number" id="LOC_16_ROW" name="LOC_16_ROW" value="%LOC_16_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_16_COL" name="LOC_16_COL" value="%LOC_16_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_16_HEIGHT" name="LOC_16_HEIGHT" value="%LOC_16_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_17_HIDDEN%>
                    <label for="LOC_17_POS" class="left-column_fill">17:</label>
                    <input type="number" id="LOC_17_ROW" name="LOC_17_ROW" value="%LOC_17_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_17_COL" name="LOC_17_COL" value="%LOC_17_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_17_HEIGHT" name="LOC_17_HEIGHT" value="%LOC_17_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_18_HIDDEN%>
                    <label for="LOC_18_POS" class="left-column_fill">18:</label>
                    <input type="number" id="LOC_18_ROW" name="LOC_18_ROW" value="%LOC_18_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_18_COL" name="LOC_18_COL" value="%LOC_18_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_18_HEIGHT" name="LOC_18_HEIGHT" value="%LOC_18_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_19_HIDDEN%>
                    <label for="LOC_19_POS" class="left-column_fill">19:</label>
                    <input type="number" id="LOC_19_ROW" name="LOC_19_ROW" value="%LOC_19_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_19_COL" name="LOC_19_COL" value="%LOC_19_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_19_HEIGHT" name="LOC_19_HEIGHT" value="%LOC_19_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_20_HIDDEN%>
                    <label for="LOC_20_POS" class="left-column_fill">20:</label>
                    <input type="number" id="LOC_20_ROW" name="LOC_20_ROW" value="%LOC_20_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_20_COL" name="LOC_20_COL" value="%LOC_20_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_20_HEIGHT" name="LOC_20_HEIGHT" value="%LOC_20_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_21_HIDDEN%>
                    <label for="LOC_21_POS" class="left-column_fill">21:</label>
                    <input type="number" id="LOC_21_ROW" name="LOC_21_ROW" value="%LOC_21_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_21_COL" name="LOC_21_COL" value="%LOC_21_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_21_HEIGHT" name="LOC_21_HEIGHT" value="%LOC_21_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_22_HIDDEN%>
                    <label for="LOC_22_POS" class="left-column_fill">22:</label>
                    <input type="number" id="LOC_22_ROW" name="LOC_22_ROW" value="%LOC_22_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_22_COL" name="LOC_22_COL" value="%LOC_22_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_22_HEIGHT" name="LOC_22_HEIGHT" value="%LOC_22_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_23_HIDDEN%>
                    <label for="LOC_23_POS" class="left-column_fill">23:</label>
                    <input type="number" id="LOC_23_ROW" name="LOC_23_ROW" value="%LOC_23_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_23_COL" name="LOC_23_COL" value="%LOC_23_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_23_HEIGHT" name="LOC_23_HEIGHT" value="%LOC_23_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_24_HIDDEN%>
                    <label for="LOC_24_POS" class="left-column_fill">24:</label>
                    <input type="number" id="LOC_24_ROW" name="LOC_24_ROW" value="%LOC_24_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_24_COL" name="LOC_24_COL" value="%LOC_24_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_24_HEIGHT" name="LOC_24_HEIGHT" value="%LOC_24_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_25_HIDDEN%>
                    <label for="LOC_25_POS" class="left-column_fill">25:</label>
                    <input type="number" id="LOC_25_ROW" name="LOC_25_ROW" value="%LOC_25_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_25_COL" name="LOC_25_COL" value="%LOC_25_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_25_HEIGHT" name="LOC_25_HEIGHT" value="%LOC_25_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_26_HIDDEN%>
                    <label for="LOC_26_POS" class="left-column_fill">26:</label>
                    <input type="number" id="LOC_26_ROW" name="LOC_26_ROW" value="%LOC_26_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_26_COL" name="LOC_26_COL" value="%LOC_26_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_26_HEIGHT" name="LOC_26_HEIGHT" value="%LOC_26_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_27_HIDDEN%>
                    <label for="LOC_27_POS" class="left-column_fill">27:</label>
                    <input type="number" id="LOC_27_ROW" name="LOC_27_ROW" value="%LOC_27_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_27_COL" name="LOC_27_COL" value="%LOC_27_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_27_HEIGHT" name="LOC_27_HEIGHT" value="%LOC_27_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_28_HIDDEN%>
                    <label for="LOC_28_POS" class="left-column_fill">28:</label>
                    <input type="number" id="LOC_28_ROW" name="LOC_28_ROW" value="%LOC_28_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_28_COL" name="LOC_28_COL" value="%LOC_28_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_28_HEIGHT" name="LOC_28_HEIGHT" value="%LOC_28_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_29_HIDDEN%>
                    <label for="LOC_29_POS" class="left-column_fill">29:</label>
                    <input type="number" id="LOC_29_ROW" name="LOC_29_ROW" value="%LOC_29_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_29_COL" name="LOC_29_COL" value="%LOC_29_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_29_HEIGHT" name="LOC_29_HEIGHT" value="%LOC_29_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_30_HIDDEN%>
                    <label for="LOC_30_POS" class="left-column_fill">30:</label>
                    <input type="number" id="LOC_30_ROW" name="LOC_30_ROW" value="%LOC_30_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_30_COL" name="LOC_30_COL" value="%LOC_30_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_30_HEIGHT" name="LOC_30_HEIGHT" value="%LOC_30_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_31_HIDDEN%>
                    <label for="LOC_31_POS" class="left-column_fill">31:</label>
                    <input type="number" id="LOC_31_ROW" name="LOC_31_ROW" value="%LOC_31_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_31_COL" name="LOC_31_COL" value="%LOC_31_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_31_HEIGHT" name="LOC_31_HEIGHT" value="%LOC_31_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_32_HIDDEN%>
                    <label for="LOC_32_POS" class="left-column_fill">32:</label>
                    <input type="number" id="LOC_32_ROW" name="LOC_32_ROW" value="%LOC_32_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_32_COL" name="LOC_32_COL" value="%LOC_32_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_32_HEIGHT" name="LOC_32_HEIGHT" value="%LOC_32_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_33_HIDDEN%>
                    <label for="LOC_33_POS" class="left-column_fill">33:</label>
                    <input type="number" id="LOC_33_ROW" name="LOC_33_ROW" value="%LOC_33_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_33_COL" name="LOC_33_COL" value="%LOC_33_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_33_HEIGHT" name="LOC_33_HEIGHT" value="%LOC_33_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_34_HIDDEN%>
                    <label for="LOC_34_POS" class="left-column_fill">34:</label>
                    <input type="number" id="LOC_34_ROW" name="LOC_34_ROW" value="%LOC_34_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_34_COL" name="LOC_34_COL" value="%LOC_34_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_34_HEIGHT" name="LOC_34_HEIGHT" value="%LOC_34_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_35_HIDDEN%>
                    <label for="LOC_35_POS" class="left-column_fill">35:</label>
                    <input type="number" id="LOC_35_ROW" name="LOC_35_ROW" value="%LOC_35_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_35_COL" name="LOC_35_COL" value="%LOC_35_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_35_HEIGHT" name="LOC_35_HEIGHT" value="%LOC_35_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_36_HIDDEN%>
                    <label for="LOC_36_POS" class="left-column_fill">36:</label>
                    <input type="number" id="LOC_36_ROW" name="LOC_36_ROW" value="%LOC_36_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_36_COL" name="LOC_36_COL" value="%LOC_36_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_36_HEIGHT" name="LOC_36_HEIGHT" value="%LOC_36_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_37_HIDDEN%>
                    <label for="LOC_37_POS" class="left-column_fill">37:</label>
                    <input type="number" id="LOC_37_ROW" name="LOC_37_ROW" value="%LOC_37_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_37_COL" name="LOC_37_COL" value="%LOC_37_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_37_HEIGHT" name="LOC_37_HEIGHT" value="%LOC_37_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_38_HIDDEN%>
                    <label for="LOC_38_POS" class="left-column_fill">38:</label>
                    <input type="number" id="LOC_38_ROW" name="LOC_38_ROW" value="%LOC_38_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_38_COL" name="LOC_38_COL" value="%LOC_38_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_38_HEIGHT" name="LOC_38_HEIGHT" value="%LOC_38_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_39_HIDDEN%>
                    <label for="LOC_39_POS" class="left-column_fill">39:</label>
                    <input type="number" id="LOC_39_ROW" name="LOC_39_ROW" value="%LOC_39_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_39_COL" name="LOC_39_COL" value="%LOC_39_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_39_HEIGHT" name="LOC_39_HEIGHT" value="%LOC_39_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_40_HIDDEN%>
                    <label for="LOC_40_POS" class="left-column_fill">40:</label>
                    <input type="number" id="LOC_40_ROW" name="LOC_40_ROW" value="%LOC_40_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_40_COL" name="LOC_40_COL" value="%LOC_40_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_40_HEIGHT" name="LOC_40_HEIGHT" value="%LOC_40_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_41_HIDDEN%>
                    <label for="LOC_41_POS" class="left-column_fill">41:</label>
                    <input type="number" id="LOC_41_ROW" name="LOC_41_ROW" value="%LOC_41_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_41_COL" name="LOC_41_COL" value="%LOC_41_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_41_HEIGHT" name="LOC_41_HEIGHT" value="%LOC_41_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_42_HIDDEN%>
                    <label for="LOC_42_POS" class="left-column_fill">42:</label>
                    <input type="number" id="LOC_42_ROW" name="LOC_42_ROW" value="%LOC_42_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_42_COL" name="LOC_42_COL" value="%LOC_42_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_42_HEIGHT" name="LOC_42_HEIGHT" value="%LOC_42_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_43_HIDDEN%>
                    <label for="LOC_43_POS" class="left-column_fill">43:</label>
                    <input type="number" id="LOC_43_ROW" name="LOC_43_ROW" value="%LOC_43_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_43_COL" name="LOC_43_COL" value="%LOC_43_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_43_HEIGHT" name="LOC_43_HEIGHT" value="%LOC_43_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_44_HIDDEN%>
                    <label for="LOC_44_POS" class="left-column_fill">44:</label>
                    <input type="number" id="LOC_44_ROW" name="LOC_44_ROW" value="%LOC_44_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_44_COL" name="LOC_44_COL" value="%LOC_44_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_44_HEIGHT" name="LOC_44_HEIGHT" value="%LOC_44_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_45_HIDDEN%>
                    <label for="LOC_45_POS" class="left-column_fill">45:</label>
                    <input type="number" id="LOC_45_ROW" name="LOC_45_ROW" value="%LOC_45_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_45_COL" name="LOC_45_COL" value="%LOC_45_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_45_HEIGHT" name="LOC_45_HEIGHT" value="%LOC_45_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_46_HIDDEN%>
                    <label for="LOC_46_POS" class="left-column_fill">46:</label>
                    <input type="number" id="LOC_46_ROW" name="LOC_46_ROW" value="%LOC_46_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_46_COL" name="LOC_46_COL" value="%LOC_46_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_46_HEIGHT" name="LOC_46_HEIGHT" value="%LOC_46_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_47_HIDDEN%>
                    <label for="LOC_47_POS" class="left-column_fill">47:</label>
                    <input type="number" id="LOC_47_ROW" name="LOC_47_ROW" value="%LOC_47_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_47_COL" name="LOC_47_COL" value="%LOC_47_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_47_HEIGHT" name="LOC_47_HEIGHT" value="%LOC_47_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_48_HIDDEN%>
                    <label for="LOC_48_POS" class="left-column_fill">48:</label>
                    <input type="number" id="LOC_48_ROW" name="LOC_48_ROW" value="%LOC_48_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_48_COL" name="LOC_48_COL" value="%LOC_48_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_48_HEIGHT" name="LOC_48_HEIGHT" value="%LOC_48_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_49_HIDDEN%>
                    <label for="LOC_49_POS" class="left-column_fill">49:</label>
                    <input type="number" id="LOC_49_ROW" name="LOC_49_ROW" value="%LOC_49_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_49_COL" name="LOC_49_COL" value="%LOC_49_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_49_HEIGHT" name="LOC_49_HEIGHT" value="%LOC_49_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_50_HIDDEN%>
                    <label for="LOC_50_POS" class="left-column_fill">50:</label>
                    <input type="number" id="LOC_50_ROW" name="LOC_50_ROW" value="%LOC_50_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_50_COL" name="LOC_50_COL" value="%LOC_50_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_50_HEIGHT" name="LOC_50_HEIGHT" value="%LOC_50_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_51_HIDDEN%>
                    <label for="LOC_51_POS" class="left-column_fill">51:</label>
                    <input type="number" id="LOC_51_ROW" name="LOC_51_ROW" value="%LOC_51_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_51_COL" name="LOC_51_COL" value="%LOC_51_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_51_HEIGHT" name="LOC_51_HEIGHT" value="%LOC_51_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_52_HIDDEN%>
                    <label for="LOC_52_POS" class="left-column_fill">52:</label>
                    <input type="number" id="LOC_52_ROW" name="LOC_52_ROW" value="%LOC_52_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_52_COL" name="LOC_52_COL" value="%LOC_52_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_52_HEIGHT" name="LOC_52_HEIGHT" value="%LOC_52_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_53_HIDDEN%>
                    <label for="LOC_53_POS" class="left-column_fill">53:</label>
                    <input type="number" id="LOC_53_ROW" name="LOC_53_ROW" value="%LOC_53_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_53_COL" name="LOC_53_COL" value="%LOC_53_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_53_HEIGHT" name="LOC_53_HEIGHT" value="%LOC_53_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_54_HIDDEN%>
                    <label for="LOC_54_POS" class="left-column_fill">54:</label>
                    <input type="number" id="LOC_54_ROW" name="LOC_54_ROW" value="%LOC_54_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_54_COL" name="LOC_54_COL" value="%LOC_54_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_54_HEIGHT" name="LOC_54_HEIGHT" value="%LOC_54_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_55_HIDDEN%>
                    <label for="LOC_55_POS" class="left-column_fill">55:</label>
                    <input type="number" id="LOC_55_ROW" name="LOC_55_ROW" value="%LOC_55_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_55_COL" name="LOC_55_COL" value="%LOC_55_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_55_HEIGHT" name="LOC_55_HEIGHT" value="%LOC_56_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_56_HIDDEN%>
                    <label for="LOC_56_POS" class="left-column_fill">56:</label>
                    <input type="number" id="LOC_56_ROW" name="LOC_56_ROW" value="%LOC_56_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_56_COL" name="LOC_56_COL" value="%LOC_56_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_56_HEIGHT" name="LOC_56_HEIGHT" value="%LOC_56_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_57_HIDDEN%>
                    <label for="LOC_57_POS" class="left-column_fill">57:</label>
                    <input type="number" id="LOC_57_ROW" name="LOC_57_ROW" value="%LOC_57_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_57_COL" name="LOC_57_COL" value="%LOC_57_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_57_HEIGHT" name="LOC_57_HEIGHT" value="%LOC_57_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_58_HIDDEN%>
                    <label for="LOC_58_POS" class="left-column_fill">58:</label>
                    <input type="number" id="LOC_58_ROW" name="LOC_58_ROW" value="%LOC_58_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_58_COL" name="LOC_58_COL" value="%LOC_58_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_58_HEIGHT" name="LOC_58_HEIGHT" value="%LOC_58_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_59_HIDDEN%>
                    <label for="LOC_59_POS" class="left-column_fill">59:</label>
                    <input type="number" id="LOC_59_ROW" name="LOC_59_ROW" value="%LOC_59_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_59_COL" name="LOC_59_COL" value="%LOC_59_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_59_HEIGHT" name="LOC_59_HEIGHT" value="%LOC_59_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_60_HIDDEN%>
                    <label for="LOC_60_POS" class="left-column_fill">60:</label>
                    <input type="number" id="LOC_60_ROW" name="LOC_60_ROW" value="%LOC_60_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_60_COL" name="LOC_60_COL" value="%LOC_60_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_60_HEIGHT" name="LOC_60_HEIGHT" value="%LOC_60_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_61_HIDDEN%>
                    <label for="LOC_61_POS" class="left-column_fill">61:</label>
                    <input type="number" id="LOC_61_ROW" name="LOC_61_ROW" value="%LOC_61_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_61_COL" name="LOC_61_COL" value="%LOC_61_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_61_HEIGHT" name="LOC_61_HEIGHT" value="%LOC_61_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_62_HIDDEN%>
                    <label for="LOC_62_POS" class="left-column_fill">62:</label>
                    <input type="number" id="LOC_62_ROW" name="LOC_62_ROW" value="%LOC_62_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_62_COL" name="LOC_62_COL" value="%LOC_62_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_62_HEIGHT" name="LOC_62_HEIGHT" value="%LOC_62_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_63_HIDDEN%>
                    <label for="LOC_63_POS" class="left-column_fill">63:</label>
                    <input type="number" id="LOC_63_ROW" name="LOC_63_ROW" value="%LOC_63_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_63_COL" name="LOC_63_COL" value="%LOC_63_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_63_HEIGHT" name="LOC_63_HEIGHT" value="%LOC_63_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_64_HIDDEN%>
                    <label for="LOC_64_POS" class="left-column_fill">64:</label>
                    <input type="number" id="LOC_64_ROW" name="LOC_64_ROW" value="%LOC_64_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_64_COL" name="LOC_64_COL" value="%LOC_64_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_64_HEIGHT" name="LOC_64_HEIGHT" value="%LOC_64_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_65_HIDDEN%>
                    <label for="LOC_65_POS" class="left-column_fill">65:</label>
                    <input type="number" id="LOC_65_ROW" name="LOC_65_ROW" value="%LOC_65_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_65_COL" name="LOC_65_COL" value="%LOC_65_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_65_HEIGHT" name="LOC_65_HEIGHT" value="%LOC_65_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_66_HIDDEN%>
                    <label for="LOC_66_POS" class="left-column_fill">66:</label>
                    <input type="number" id="LOC_66_ROW" name="LOC_66_ROW" value="%LOC_66_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_66_COL" name="LOC_66_COL" value="%LOC_66_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_66_HEIGHT" name="LOC_66_HEIGHT" value="%LOC_66_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_67_HIDDEN%>
                    <label for="LOC_67_POS" class="left-column_fill">67:</label>
                    <input type="number" id="LOC_67_ROW" name="LOC_67_ROW" value="%LOC_67_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_67_COL" name="LOC_67_COL" value="%LOC_67_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_67_HEIGHT" name="LOC_67_HEIGHT" value="%LOC_67_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_68_HIDDEN%>
                    <label for="LOC_68_POS" class="left-column_fill">68:</label>
                    <input type="number" id="LOC_68_ROW" name="LOC_68_ROW" value="%LOC_68_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_68_COL" name="LOC_68_COL" value="%LOC_68_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_68_HEIGHT" name="LOC_68_HEIGHT" value="%LOC_68_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_69_HIDDEN%>
                    <label for="LOC_69_POS" class="left-column_fill">69:</label>
                    <input type="number" id="LOC_69_ROW" name="LOC_69_ROW" value="%LOC_69_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_69_COL" name="LOC_69_COL" value="%LOC_69_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_69_HEIGHT" name="LOC_69_HEIGHT" value="%LOC_69_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_70_HIDDEN%>
                    <label for="LOC_70_POS" class="left-column_fill">70:</label>
                    <input type="number" id="LOC_70_ROW" name="LOC_70_ROW" value="%LOC_70_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_70_COL" name="LOC_70_COL" value="%LOC_70_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_70_HEIGHT" name="LOC_70_HEIGHT" value="%LOC_70_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_71_HIDDEN%>
                    <label for="LOC_71_POS" class="left-column_fill">71:</label>
                    <input type="number" id="LOC_71_ROW" name="LOC_71_ROW" value="%LOC_71_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_71_COL" name="LOC_71_COL" value="%LOC_71_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_71_HEIGHT" name="LOC_71_HEIGHT" value="%LOC_71_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_72_HIDDEN%>
                    <label for="LOC_72_POS" class="left-column_fill">72:</label>
                    <input type="number" id="LOC_72_ROW" name="LOC_72_ROW" value="%LOC_72_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_72_COL" name="LOC_72_COL" value="%LOC_72_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_72_HEIGHT" name="LOC_72_HEIGHT" value="%LOC_72_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_73_HIDDEN%>
                    <label for="LOC_73_POS" class="left-column_fill">73:</label>
                    <input type="number" id="LOC_73_ROW" name="LOC_73_ROW" value="%LOC_73_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_73_COL" name="LOC_73_COL" value="%LOC_73_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_73_HEIGHT" name="LOC_73_HEIGHT" value="%LOC_73_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_74_HIDDEN%>
                    <label for="LOC_74_POS" class="left-column_fill">74:</label>
                    <input type="number" id="LOC_74_ROW" name="LOC_74_ROW" value="%LOC_74_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_74_COL" name="LOC_74_COL" value="%LOC_74_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_74_HEIGHT" name="LOC_74_HEIGHT" value="%LOC_74_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_75_HIDDEN%>
                    <label for="LOC_75_POS" class="left-column_fill">75:</label>
                    <input type="number" id="LOC_75_ROW" name="LOC_75_ROW" value="%LOC_75_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_75_COL" name="LOC_75_COL" value="%LOC_75_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_75_HEIGHT" name="LOC_75_HEIGHT" value="%LOC_75_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_76_HIDDEN%>
                    <label for="LOC_76_POS" class="left-column_fill">76:</label>
                    <input type="number" id="LOC_76_ROW" name="LOC_76_ROW" value="%LOC_76_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_76_COL" name="LOC_76_COL" value="%LOC_76_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_76_HEIGHT" name="LOC_76_HEIGHT" value="%LOC_76_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_77_HIDDEN%>
                    <label for="LOC_77_POS" class="left-column_fill">77:</label>
                    <input type="number" id="LOC_77_ROW" name="LOC_77_ROW" value="%LOC_77_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_77_COL" name="LOC_77_COL" value="%LOC_77_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_77_HEIGHT" name="LOC_77_HEIGHT" value="%LOC_77_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_78_HIDDEN%>
                    <label for="LOC_78_POS" class="left-column_fill">78:</label>
                    <input type="number" id="LOC_78_ROW" name="LOC_78_ROW" value="%LOC_78_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_78_COL" name="LOC_78_COL" value="%LOC_78_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_78_HEIGHT" name="LOC_78_HEIGHT" value="%LOC_78_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_79_HIDDEN%>
                    <label for="LOC_79_POS" class="left-column_fill">79:</label>
                    <input type="number" id="LOC_79_ROW" name="LOC_79_ROW" value="%LOC_79_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_79_COL" name="LOC_79_COL" value="%LOC_79_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_79_HEIGHT" name="LOC_79_HEIGHT" value="%LOC_79_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_80_HIDDEN%>
                    <label for="LOC_80_POS" class="left-column_fill">80:</label>
                    <input type="number" id="LOC_80_ROW" name="LOC_80_ROW" value="%LOC_80_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_80_COL" name="LOC_80_COL" value="%LOC_80_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_80_HEIGHT" name="LOC_80_HEIGHT" value="%LOC_80_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_81_HIDDEN%>
                    <label for="LOC_81_POS" class="left-column_fill">81:</label>
                    <input type="number" id="LOC_81_ROW" name="LOC_81_ROW" value="%LOC_81_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_81_COL" name="LOC_81_COL" value="%LOC_81_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_81_HEIGHT" name="LOC_81_HEIGHT" value="%LOC_81_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_82_HIDDEN%>
                    <label for="LOC_82_POS" class="left-column_fill">82:</label>
                    <input type="number" id="LOC_82_ROW" name="LOC_82_ROW" value="%LOC_82_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_82_COL" name="LOC_82_COL" value="%LOC_82_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_82_HEIGHT" name="LOC_82_HEIGHT" value="%LOC_82_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_83_HIDDEN%>
                    <label for="LOC_83_POS" class="left-column_fill">83:</label>
                    <input type="number" id="LOC_83_ROW" name="LOC_83_ROW" value="%LOC_83_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_83_COL" name="LOC_83_COL" value="%LOC_83_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_83_HEIGHT" name="LOC_83_HEIGHT" value="%LOC_83_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_84_HIDDEN%>
                    <label for="LOC_84_POS" class="left-column_fill">84:</label>
                    <input type="number" id="LOC_84_ROW" name="LOC_84_ROW" value="%LOC_84_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_84_COL" name="LOC_84_COL" value="%LOC_84_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_84_HEIGHT" name="LOC_84_HEIGHT" value="%LOC_84_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_85_HIDDEN%>
                    <label for="LOC_85_POS" class="left-column_fill">85:</label>
                    <input type="number" id="LOC_85_ROW" name="LOC_85_ROW" value="%LOC_85_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_85_COL" name="LOC_85_COL" value="%LOC_85_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_85_HEIGHT" name="LOC_85_HEIGHT" value="%LOC_85_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_86_HIDDEN%>
                    <label for="LOC_86_POS" class="left-column_fill">86:</label>
                    <input type="number" id="LOC_86_ROW" name="LOC_86_ROW" value="%LOC_86_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_86_COL" name="LOC_86_COL" value="%LOC_86_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_86_HEIGHT" name="LOC_86_HEIGHT" value="%LOC_86_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_87_HIDDEN%>
                    <label for="LOC_87_POS" class="left-column_fill">87:</label>
                    <input type="number" id="LOC_87_ROW" name="LOC_87_ROW" value="%LOC_87_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_87_COL" name="LOC_87_COL" value="%LOC_87_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_87_HEIGHT" name="LOC_87_HEIGHT" value="%LOC_87_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_88_HIDDEN%>
                    <label for="LOC_88_POS" class="left-column_fill">88:</label>
                    <input type="number" id="LOC_88_ROW" name="LOC_88_ROW" value="%LOC_88_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_88_COL" name="LOC_88_COL" value="%LOC_88_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_88_HEIGHT" name="LOC_88_HEIGHT" value="%LOC_88_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_89_HIDDEN%>
                    <label for="LOC_89_POS" class="left-column_fill">89:</label>
                    <input type="number" id="LOC_89_ROW" name="LOC_89_ROW" value="%LOC_89_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_89_COL" name="LOC_89_COL" value="%LOC_89_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_89_HEIGHT" name="LOC_89_HEIGHT" value="%LOC_89_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_90_HIDDEN%>
                    <label for="LOC_90_POS" class="left-column_fill">90:</label>
                    <input type="number" id="LOC_90_ROW" name="LOC_90_ROW" value="%LOC_90_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_90_COL" name="LOC_90_COL" value="%LOC_90_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_90_HEIGHT" name="LOC_90_HEIGHT" value="%LOC_90_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_91_HIDDEN%>
                    <label for="LOC_91_POS" class="left-column_fill">91:</label>
                    <input type="number" id="LOC_91_ROW" name="LOC_91_ROW" value="%LOC_91_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_91_COL" name="LOC_91_COL" value="%LOC_91_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_91_HEIGHT" name="LOC_91_HEIGHT" value="%LOC_91_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_92_HIDDEN%>
                    <label for="LOC_92_POS" class="left-column_fill">92:</label>
                    <input type="number" id="LOC_92_ROW" name="LOC_92_ROW" value="%LOC_92_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_92_COL" name="LOC_92_COL" value="%LOC_92_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_92_HEIGHT" name="LOC_92_HEIGHT" value="%LOC_92_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_93_HIDDEN%>
                    <label for="LOC_93_POS" class="left-column_fill">93:</label>
                    <input type="number" id="LOC_93_ROW" name="LOC_93_ROW" value="%LOC_93_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_93_COL" name="LOC_93_COL" value="%LOC_93_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_93_HEIGHT" name="LOC_93_HEIGHT" value="%LOC_93_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_94_HIDDEN%>
                    <label for="LOC_94_POS" class="left-column_fill">94:</label>
                    <input type="number" id="LOC_94_ROW" name="LOC_94_ROW" value="%LOC_94_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_94_COL" name="LOC_94_COL" value="%LOC_94_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_94_HEIGHT" name="LOC_94_HEIGHT" value="%LOC_94_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_95_HIDDEN%>
                    <label for="LOC_95_POS" class="left-column_fill">95:</label>
                    <input type="number" id="LOC_95_ROW" name="LOC_95_ROW" value="%LOC_95_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_95_COL" name="LOC_95_COL" value="%LOC_95_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_95_HEIGHT" name="LOC_95_HEIGHT" value="%LOC_95_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_96_HIDDEN%>
                    <label for="LOC_96_POS" class="left-column_fill">96:</label>
                    <input type="number" id="LOC_96_ROW" name="LOC_96_ROW" value="%LOC_96_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_96_COL" name="LOC_96_COL" value="%LOC_96_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_96_HEIGHT" name="LOC_96_HEIGHT" value="%LOC_96_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_97_HIDDEN%>
                    <label for="LOC_97_POS" class="left-column_fill">97:</label>
                    <input type="number" id="LOC_97_ROW" name="LOC_97_ROW" value="%LOC_97_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_97_COL" name="LOC_97_COL" value="%LOC_97_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_97_HEIGHT" name="LOC_97_HEIGHT" value="%LOC_97_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_98_HIDDEN%>
                    <label for="LOC_98_POS" class="left-column_fill">98:</label>
                    <input type="number" id="LOC_98_ROW" name="LOC_98_ROW" value="%LOC_98_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_98_COL" name="LOC_98_COL" value="%LOC_98_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_98_HEIGHT" name="LOC_98_HEIGHT" value="%LOC_98_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>

                <div class="pos-grid" %LOC_99_HIDDEN%>
                    <label for="LOC_99_POS" class="left-column_fill">99:</label>
                    <input type="number" id="LOC_99_ROW" name="LOC_99_ROW" value="%LOC_99_ROW%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_99_COL" name="LOC_99_COL" value="%LOC_99_COL%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                    <input type="number" id="LOC_99_HEIGHT" name="LOC_99_HEIGHT" value="%LOC_99_HEIGHT%" min="0" max="254" onfocus="updateFocusedElement(this.id)">
                </div>
                <input type="submit" value="Update">
            </form>
        </div>
        <div></div>
    </div>
</body>

</html>
%LAST_LINE%)rawliteral";