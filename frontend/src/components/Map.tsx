// Code insipired by: https://www.npmjs.com/package/@react-google-maps/api
import React from "react";
import { GoogleMap, useJsApiLoader, Marker } from "@react-google-maps/api";
import { LocationData } from "../types/types";
import { GOOGLE_MAPS_API_KEY } from "../secrets";

const containerStyle = {
  width: "450px",
  height: "450px",
  boxShadow: "0px 0px 3px #FAFAFA",
};

function MapComponent({ lat, lng }: LocationData) {
  const { isLoaded } = useJsApiLoader({
    id: "google-map-script",
    googleMapsApiKey: GOOGLE_MAPS_API_KEY,
  });

  const center = {
    lat,
    lng,
  };

  const [map, setMap] = React.useState(null);

  const onLoad = React.useCallback(function callback(map: any) {
    map.setZoom(15);
    setMap(map);
  }, []);

  const onUnmount = React.useCallback(function callback(map: any) {
    setMap(null);
  }, []);

  return isLoaded ? (
    <GoogleMap
      mapContainerStyle={containerStyle}
      center={center}
      onLoad={onLoad}
      onUnmount={onUnmount}
    >
      <Marker position={{ lat, lng }} />
    </GoogleMap>
  ) : (
    <></>
  );
}

export default MapComponent;
