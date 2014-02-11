Description
-----------
The module extends the covers functionality in Ting by providing default covers per material type as well as querying for covers for certain extra sources (Historisk Atlas and Danske Billeder are currently supported).

The module has been created to be compatible with DDB CMS.
 
Installation
------------

Download and enable the module.

Configuration of the module
---------------------------

### Settings in the administration section (admin/config/ting/covers/defaults)

* "Default covers > Uploaded covers": Here, covers are uploaded. The rest of the settings can only be set when some covers have been uploaded.

* "Default covers > Covers available for setting defaults": Displays the uploaded covers for easy visual reference.

* "Default covers > Default cover ": The default cover is the fallback cover image used when no specific cover has been set. 
 
* "Default covers > Type specific default covers": Each well type is present here, and is it possible to set a cover image for each one specifically. If you do not see the expected amount of types in here, go to Admin > Configuration > Ting > Online types and URL labels (admin/config/ting/online_types), and click "Update from datawell".
