<!DOCTYPE html>
<html>
  <head>
    <title>Gps</title>
    <meta name="author" content="Davide D'Osvaldo">
    <meta charset="UTF-8">
    <meta name="viewport" content="initial-scale=1, minimum-scale=1, maximum-scale=1, user-scalable=no" />
    <!--caricamento dei file css-->
    <style>@import url(css/informations.css);</style>
    <style>@import url(css/links.css);</style>
    <style>@import url(css/stili.css);</style>
    <style>@import url(css/contenuto.css);</style>
    <!--caricamento dei file javascript-->
    <script type="text/JavaScript" src="javascript/jquery.min.js"></script>
    <script type="text/JavaScript" src="javascript/global.js"></script>
    <script type="text/JavaScript" src="javascript/resize.js"></script>
    <script type="text/JavaScript" src="javascript/updating.js"></script>
    <script type="text/JavaScript" src="javascript/compassIndicator.js"></script>
    <script type="text/JavaScript">
      $(document).ready(function(){
        $('#a3').addClass('selected');
      });
      //funzione di aggiornamento dei dati
      function refreshInformations(){
        compass(cogV, 'cog', '.left.top', 0);
        compass(mhV, 'mh', '.left.bottom', 2);
        $('#sow').text(sowV);
        $('#sog').text(sogV);
        $('#lat').text(lat);
        $('#long').text(lon);
      }
      (function($){
        //funzione di inizializzazione delle dimensioni e dei dati
        $(document).ready(function(){
          resizeWindow();
          refreshInformations();
          updatingData();
        });
        //funzione che si attiva al ridimensionamento della pagina
        $(window).resize(function(){
          resizeWindow();
          refreshInformations();
        });
      })(jQuery);
    </script>
  </head>
  <body>
    <?php
      include 'Informations.php';
    ?>
    <div id="left">
      <?php
        include 'links.php';
      ?>
    </div>
    <table id="contenuto">
      <tr>
        <td class="left top">
          <div class="name">CURSE OVER GROUND</div>
          <canvas id="cog" height="0" width="0"></canvas>
        </td>
        <td class="right top">
          <div class="name">SPEED OVER<br>GROUND [kt]</div>
          <div class="value" id="sog"></div>
        </td>
      </tr>
      <tr>
        <td class="left bottom">
          <div class="name">MAGNETIC HEADING</div>
          <canvas id="mh" height="0" width="0"></canvas>
        </td>
        <td class="right bottom">
          <div class="name">SPEED OVER<br>WATER [kt]</div>
          <div class="value" id="sow"></div>
        </td>
      </tr>
    </table>
    <div id="right">
      <?php
        include 'links2.php';
      ?>
    </div>
  </body>
</html>