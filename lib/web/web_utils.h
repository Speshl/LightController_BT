#include <Arduino.h>

static const char utils_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
    <style></style>
</head>
<body>

<form action="/save" method="post">
    <input type="submit" value="Write Changes"> 
</form>

<form action="/defaultLocations" method="post">
    <input type="submit" value="Reset To Default Locations"> 
</form>

<form action="/import" method="post" enctype="multipart/form-data">
    <input type="file" id="myFile" name="filename">
    <input type="submit">
  </form>
<button><a href="/export" download="test.json">Export</a></button>
</body>
</html>
%LAST_LINE%)rawliteral";