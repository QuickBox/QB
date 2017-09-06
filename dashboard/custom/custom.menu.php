<?php

///////////////////////////////////////////////////
// BEGIN TUTORIAL ON ADDING IN YOUR CUSTOM LINKS //
///////////////////////////////////////////////////
//
// ..examples..
//
// $appURL = "https://" . $_SERVER['HTTP_HOST'] . ":0000/"; # links to custom app install on your server
// $mysiteURL = "https://www.mydomain.com/"; # links to custom external source of your choosing
//
// Now, let's add them to the menu!
//
// <li><a class="grayscale" href="<?php echo "$appURL"; ?//>" target="_blank"><img src="img/brands/app.png" class="brand-ico"> <span>Custom App</span></a></li>
// <li><a class="grayscale" href="<?php echo "$mysiteURL"; ?//>" target="_blank"><img src="img/brands/sitelogo.png" class="brand-ico"> <span>My External Site</span></a></li>
//
// You'll notice in the examples above that after (for instance) "$appURL"; ?
// there are two forward slashes (//). Remove these on live examples.
// These were included to avoid php collisions within the examples.
//
//----------------------
// Adding Logo Images //
//----------------------
//
// You can add custom logos to your created menu items by placing the logos
// within the /srv/rutorrent/home/img/brands/ directory. For best results, add
// images to be no greater than 128x128 pixels in size.
// Remember to change the image source to the name you have given your logo icon.
// Any image format is acceptable: (jpg,png,svg,gif)
// <img src="img/brands/CHANGETHIS.png" class="brand-ico">
//
//
///////////////////////////////////////////////////
//  END TUTORIAL ON ADDING IN YOUR CUSTOM LINKS  //
///////////////////////////////////////////////////


// INSERT YOUR CUSTOM EXTERNAL or INTERNAL LINKS HERE! //
$quickboxURL = "https://quickbox.io/donate/";

?> <!-- Do not remove this closing (?>) -->


<!-- // INSERT YOUR CUSTOM EXTERNAL or INTERNAL MENU ITEMS HERE! // -->
<li><a href="<?php echo "$quickboxURL"; ?>" class="grayscale" target="_blank"><i class="fa fa-heart text-danger"></i> <span><?php echo T('DONATE'); ?></span></a></li>
