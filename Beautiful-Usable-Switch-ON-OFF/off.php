<!DOCTYPE html>
<html>
    <head>
        <meta charset="UTF-8">
        <title></title>
        <link rel="stylesheet" type="text/css" href="style.css">
    </head>
    <body>
        
        <!-- Basic switch -->

    <div class="toggle basic-switch">
        <label class="switch switch_type1" role="switch">
            <input onchange="updateOnOff(this);" type="checkbox" class="switch__toggle" >
            <span class="switch__label"></span>
        </label>
    </div>

    <!-- Day/Night switch -->

    <div class="toggle toggle--daynight">
            <input onchange="updateOnOff(this);" type="checkbox" id="toggle--daynight" class="toggle--checkbox">
            <label class="toggle--btn" for="toggle--daynight"><span class="toggle--feature"></span></label>
    </div>

    <!-- Push button switch -->

    <div class="toggle toggle--push">
            <input onchange="updateOnOff(this);" type="checkbox" id="toggle--push" class="toggle--checkbox">
            <label class="toggle--btn" for="toggle--push" data-label-on="on"  data-label-off="off"></label>
    </div>


    <script>
        function updateOnOff(elt) {
            if (elt.checked) {
                window.location.assign('on.php');
            } else {
                window.location.assign('off.php');
            }
        }
    </script>
    </body>
</html>
