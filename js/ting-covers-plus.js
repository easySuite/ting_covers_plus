(function($) {
  "use strict";

  // Helper function to get information about a given cover place holder.
  var ting_covers_extract_data = function(e) {
    var local_id = $(e).data('ting-cover-object-id');
    var image_style = $(e).data('ting-cover-style');

    if (!local_id) {
      return false;
    }
    return {
      local_id : local_id,
      image_style : image_style
    };
  };

  var ting_cover_insert = function(covers) {
    $.each(covers, function(index, cover_info) {
      var $cover_block = $('.ting-cover[data-ting-cover-object-id="' + cover_info.local_id + '"]');

      if (cover_info.url) {
        $cover_block.html('<img src="' + cover_info.url + '"/>');

        if (typeof cover_info.class !== 'undefined' && cover_info.class.length > 0) {
          $cover_block.addClass(cover_info.class);
        }
      }
    });

    // Remove no image picture from covers.
    if (Drupal.settings.ting_covers_plus != undefined && Drupal.settings.ting_covers_plus.ting_covers_plus_hide_covers === 1) {
      $('.ting-cover').each(function(index, element) {
        var $img = $(element).find('img');
        var cur_el = $(element);
        if ($img.length === 0 || cur_el.hasClass('ting-covers-plus-default')) {
          var el = this;
          // Add class for styling purposes
          $(el).closest('.ting-object').addClass('no-cover');
          $(el).remove();
        }
      });
    }
  };

  Drupal.behaviors.tingCoversPlus = {
    attach: function(context) {
      // Assemble information regarding covers.
      var cover_data = [];
      // Extract cover information from the dom.
      $('.ting-cover.ting-cover-processing:not(.ting-cover-processed)', context).each(function(index, element) {
        cover_data.push(ting_covers_extract_data(element));
      });

      if (cover_data.length > 0) {
        //Retrieve covers
        var request = $.ajax({
          url: Drupal.settings.basePath + 'ting/coversplus',
          type: 'POST',
          data: {
            coverData: cover_data
          },
          dataType: 'json',
          success: ting_cover_insert,
          // Update processing state.
          complete: function(request, status) {
            var processing = $('.ting-cover-processing', context);
            if (status === 'success') {
              processing.addClass('ting-cover-processed');
            }
            processing.removeClass('ting-cover-processing');
          }
        });

        // Associate the request with the context so we can abort the request if
        // the context is detached removed before completion.
        $(context).data('request', request);
      }
    },
    detach: function(context) {
      // If we have a request associated with the context then abort it.
      var request = $(context).data('request');
      if (request) {
        request.abort();
      }
    }
  };
}(jQuery));
