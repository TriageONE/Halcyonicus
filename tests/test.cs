public string GetMerakiDataTest() {
    string MerakiData = "";
    string currentuser = Authentication.getCurrentUser(Request);
    string merakiapikey = "";

    try {
      merakiapikey = ConfigurationManager.AppSettings["MerakiAPIKey"];
      var sourceClient = new WebClient();
      sourceClient.Headers.Add("X-Cisco-Meraki-API-Key", merakiapikey);
      MerakiData = sourceClient.DownloadString(https: //api.meraki.com/api/v1/organizations/238018/devices);
      }
      catch (Exception ex) {
        Models.Database.LogError(ex, currentuser, "");
      }
      return MerakiData;
    }
}

public string GetMerakiDataTest() {
    string currentuser = Authentication.getCurrentUser(Request);
    string merakiapikey = "";
    try {
      string uri = "https: //api.meraki.com/api/v1/organizations/238018/devices"; /*quotes */
        merakiapikey = ConfigurationManager.AppSettings["MerakiAPIKey"];

      HttpWebRequest request = (HttpWebRequest) WebRequest.Create(uri);
      request.AutomaticDecompression = DecompressionMethods.GZip | DecompressionMethods.Deflate;
      request.Headers["X-Cisco-Meraki-API-Key"] = merakiapikey;

      using(HttpWebResponse response = (HttpWebResponse) request.GetResponse())
      using(Stream stream = response.GetResponseStream())

      using(StreamReader reader = new StreamReader(stream)) {
        return reader.ReadToEnd();
      }
    } catch (Exception ex) {
      Models.Database.LogError(ex, currentuser, "");
    }

    return "Error";
    }