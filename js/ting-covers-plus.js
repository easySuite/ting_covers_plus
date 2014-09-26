(function($) {
  "use strict";

  // Helper function to get information about a given cover place holder.
  var ting_covers_extract_data = function(e) {
    var classname = $(e).attr('class');
    var local_id = classname.match(/ting-cover-object-id-(\S+)/);
    var image_style = classname.match(/ting-cover-style-(\S+)/);
    var owner_id = classname.match(/ting-cover-owner-id-(\S+)/);
    var mtype = classname.match(/ting-cover-mtype-(\S+)/);
    var source = classname.match(/ting-cover-source-(\S+)/);
    if (!local_id) {
      return false;
    }
    return {
      local_id : local_id[1],
      owner_id : owner_id[1],
      image_style : image_style[1],
      mtype : mtype[1],
      source : source[1]
    };
  };

  var ting_cover_insert = function(covers) {
    $.each(covers, function(index, cover_info) {
      var $cover_block = $('.ting-cover-processing' + '.ting-cover-object-id-' + cover_info.local_id + '.ting-cover-style-' + cover_info.image_style);
      $cover_block.html('<img src="' + cover_info.url + '"/>');
      if (typeof cover_info.class !== 'undefined' && cover_info.class.length > 0) {
        $cover_block.addClass(cover_info.class);
      }
    });

    // Remove no image picture from covers.
    if (Drupal.settings.ting_covers_plus.ting_covers_plus_hide_covers === 1) {
      $('.ting-cover').each(function(index, element) {
        if ($(element).find('img').length === 0 || $(element).hasClass('ting-covers-plus-default')) {
          this.remove();
        }
      })
    }
  };

  Drupal.behaviors.tingCovers = {
    attach: function(context) {
      // Assemble information regarding covers.
      var cover_data = [];

      // Extract cover information from the dom.
      $('.ting-cover:not(.ting-cover-processing, .ting-cover-processed)', context).each(function(index, element) {
        cover_data.push(ting_covers_extract_data(element));
      }).addClass('ting-cover-processing');

      // Get all images without covers, even if cache exist.
      if (cover_data.length === 0) {
        $('.ting-cover').each(function(index, element) {
          if ($(element).find('img').length === 0) {
            cover_data.push(ting_covers_extract_data(element));
          }
        }).addClass('ting-cover-processing');
      }

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
