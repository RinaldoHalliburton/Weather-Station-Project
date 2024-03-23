<template>
  <v-container align="center">
    <v-row>
      <v-col align="center">
        <v-card
          class="mb-5"
          style="max-width: 500px"
          color="white"
          title="TIME & DATE"
        >
          <v-card-item>
            <span>
              {{ currentDate }}
            </span>
            <span class="text-onContainer text-h3">
              {{ time }}
            </span>
          </v-card-item>
        </v-card>
      </v-col>
    </v-row>
    <v-row class="row">
      <v-col>
        <v-card
          class="mb-5"
          style="max-width: 500px"
          color="red"
          subtitle="Temperature"
        >
          <v-card-item>
            <span class="text-onContainer text-h3">
              {{ temperature }}
            </span>
          </v-card-item>
        </v-card>
        <v-card
          class="mb-5"
          style="max-width: 500px"
          color="green"
          subtitle="Humidity"
        >
          <v-card-item>
            <span class="text-onContainer text-h3">
              {{ humidity }}
            </span>
          </v-card-item>
        </v-card>
        <v-card
          class="mb-5"
          style="max-width: 500px"
          color="blue"
          subtitle="Heat Index"
        >
          <v-card-item>
            <span class="text-onContainer text-h3">
              {{ heatindex }}
            </span>
          </v-card-item>
        </v-card>
      </v-col>
      <v-col>
        <v-card
          class="mb-5"
          style="max-width: 500px"
          color="yellow"
          subtitle="Pressure"
        >
          <v-card-item>
            <span class="text-onContainer text-h3">
              {{ pressure }}
            </span>
          </v-card-item>
        </v-card>
        <v-card
          class="mb-5"
          style="max-width: 500px"
          color="orange"
          subtitle="Soil Moisture"
        >
          <v-card-item>
            <span class="text-onTertiaryContainer text-h3">
              {{ soilmoisture }}
            </span>
          </v-card-item>
        </v-card>
        <v-card
          class="mb-5"
          style="max-width: 500px"
          color="purple"
          subtitle="Altitude"
        >
          <v-card-item>
            <span class="text-onSecondaryContainer text-h3">
              {{ altitude }}
            </span>
          </v-card-item>
        </v-card>
      </v-col>
    </v-row>
    <v-row>
      <v-col>
        <v-btn text class="mr-2" color="grey" @click="toggleNormal"
          >SI Units</v-btn
        >
        <v-btn color="grey" @click="toggleAmerican">Imperial</v-btn>
      </v-col>
    </v-row>

    <v-row class="row" justify="start">
      <v-col class="col-9">
        <figure class="highcharts-figure">
          <div id="container1"></div>
        </figure>
      </v-col>
    </v-row>
  </v-container>
</template>

<script setup>
/** JAVASCRIPT HERE */

// IMPORTS

import { useMqttStore } from "@/store/mqttStore"; // Import Mqtt Store
import { storeToRefs } from "pinia";

// Highcharts, Load the exporting module and Initialize exporting module.
import Highcharts from "highcharts";
import more from "highcharts/highcharts-more";
import Exporting from "highcharts/modules/exporting";
Exporting(Highcharts);
more(Highcharts);

import {
  ref,
  reactive,
  watch,
  onMounted,
  onBeforeUnmount,
  computed,
} from "vue";
import { useRoute, useRouter } from "vue-router";

// VARIABLES
const router = useRouter();
const route = useRoute();

const Mqtt = useMqttStore();
const { payload, payloadTopic } = storeToRefs(Mqtt);

const Chart = ref(null); // Chart object

const points = ref(15); // Specify the quantity of points to be shown on the live graph simultaneously.
const shift = ref(false); // Delete a point from the left side and append a new point to the right side of the graph.
var imperial = false;
var normal = true;
const time = ref(new Date().toLocaleTimeString());
const currentDate = ref(new Date().toLocaleDateString());

// FUNCTIONS

onMounted(() => {
  // THIS FUNCTION IS CALLED AFTER THIS COMPONENT HAS BEEN MOUNTED
  // CreateCharts();
  CreateChart();
  setInterval(updateTime, 1000);
  setUpdateTimeout();

  Mqtt.connect(); // Connect to Broker located on the backend
  setTimeout(() => {
    // Subscribe to each topic
    Mqtt.subscribe("620153775");
    Mqtt.subscribe("620153775_sub");
  }, 3000);
});

onBeforeUnmount(() => {
  // THIS FUNCTION IS CALLED RIGHT BEFORE THIS COMPONENT IS UNMOUNTED
  // unsubscribe from all topics
  Mqtt.unsubcribeAll();
});

const toggleAmerican = async () => {
  imperial = true;
  normal = false;
};
const toggleNormal = async () => {
  imperial = false;
  normal = true;
};

const updateTime = () => {
  time.value = new Date().toLocaleTimeString();
};

const updateDate = () => {
  currentDate.value = new Date().toLocaleDateString();
  // Set the next update for just after midnight
  setUpdateTimeout();
};
const setUpdateTimeout = () => {
  const now = new Date();
  // Calculate time until midnight
  const timeUntilMidnight =
    new Date(now.getFullYear(), now.getMonth(), now.getDate() + 1) - now;
  setTimeout(updateDate, timeUntilMidnight);
};

const CreateChart = async () => {
  //  CHART
  Chart.value = Highcharts.chart("container1", {
    chart: {
      events: {
        load() {
          console.log(this.options.colors);
        },
      },
    },
    chart: { zoomType: "x" },
    title: { text: "Weather Station Analysis", align: "left" },
    yAxis: {
      title: {
        text: "Data",
        style: { color: "#000000" },
      },
      labels: { format: "{value}" },
    },
    xAxis: {
      type: "datetime",
      title: { text: "Time", style: { color: "#000000" } },
    },
    tooltip: { shared: true },
    colors: ["red", "green", "blue", "yellow", "orange", "purple"],
    series: [
      {
        name: "Temperature",
        type: "spline",
        data: [],
        turboThreshold: 0,
      },
      {
        name: "Humidity",
        type: "spline",
        data: [],
        turboThreshold: 0,
      },
      {
        name: "Heatindex",
        type: "spline",
        data: [],
        turboThreshold: 0,
      },
      {
        name: "Pressure",
        type: "spline",
        data: [],
        turboThreshold: 0,
      },
      {
        name: "Soilmoisture",
        type: "spline",
        data: [],
        turboThreshold: 0,
      },
      {
        name: "Altitude",
        type: "spline",
        data: [],
        turboThreshold: 0,
      },
    ],
  });
};

// COMPUTED PROPERTIES
const temperature = computed(() => {
  if (!!payload.value) {
    if (imperial == false && normal == true) {
      return `${payload.value.temperature.toFixed(2)} °C`;
    }
    if (imperial == true && normal == false) {
      return `${(payload.value.temperature * 1.8 + 32).toFixed(2)} °F`;
    }
  }
});
const heatindex = computed(() => {
  if (!!payload.value) {
    if (imperial == false && normal == true) {
      return `${payload.value.heatindex.toFixed(2)} °C`;
    }
    if (imperial == true && normal == false) {
      return `${(payload.value.heatindex * 1.8 + 32).toFixed(2)} °F`;
    }
  }
});
const humidity = computed(() => {
  if (!!payload.value) {
    return `${payload.value.humidity.toFixed(2)} %`;
  }
});
const pressure = computed(() => {
  if (!!payload.value) {
    if (imperial == false && normal == true) {
      return `${payload.value.pressure.toFixed(2)} kPa`;
    }
    if (imperial == true && normal == false) {
      return `${(payload.value.pressure / 3.386).toFixed(2)} Hg`;
    }
  }
});
const soilmoisture = computed(() => {
  if (!!payload.value) {
    return `${payload.value.soilmoisture.toFixed(0)} %`;
  }
});
const altitude = computed(() => {
  if (!!payload.value) {
    if (imperial == false && normal == true) {
      return `${payload.value.altitude.toFixed(2)} m`;
    }
    if (imperial == true && normal == false) {
      return `${(payload.value.altitude * 1.094).toFixed(2)} yd`;
    }
  }
});

// WATCHERS
watch(payload, (data) => {
  if (points.value > 0) {
    points.value--;
  } else {
    shift.value = true;
  }
  if (imperial == false && normal == true) {
    Chart.value.series[0].addPoint(
      { y: parseFloat(data.temperature.toFixed(2)), x: data.timestamp * 1000 },
      true,
      shift.value
    );
    Chart.value.series[2].addPoint(
      { y: parseFloat(data.heatindex.toFixed(2)), x: data.timestamp * 1000 },
      true,
      shift.value
    );
    Chart.value.series[3].addPoint(
      { y: parseFloat(data.pressure.toFixed(2)), x: data.timestamp * 1000 },
      true,
      shift.value
    );
    Chart.value.series[5].addPoint(
      { y: parseFloat(data.altitude.toFixed(2)), x: data.timestamp * 1000 },
      true,
      shift.value
    );
  }
  if (imperial == true && normal == false) {
    Chart.value.series[0].addPoint(
      {
        y: parseFloat((data.temperature * 1.8 + 32).toFixed(2)),
        x: data.timestamp * 1000,
      },
      true,
      shift.value
    );
    Chart.value.series[2].addPoint(
      {
        y: parseFloat((data.heatindex * 1.8 + 32).toFixed(2)),
        x: data.timestamp * 1000,
      },
      true,
      shift.value
    );
    Chart.value.series[3].addPoint(
      {
        y: parseFloat((data.pressure / 3.386).toFixed(2)),
        x: data.timestamp * 1000,
      },
      true,
      shift.value
    );
    Chart.value.series[5].addPoint(
      {
        y: parseFloat((data.altitude * 1.094).toFixed(2)),
        x: data.timestamp * 1000,
      },
      true,
      shift.value
    );
  }
  Chart.value.series[1].addPoint(
    { y: parseFloat(data.humidity.toFixed(2)), x: data.timestamp * 1000 },
    true,
    shift.value
  );

  Chart.value.series[4].addPoint(
    { y: parseFloat(data.soilmoisture.toFixed(0)), x: data.timestamp * 1000 },
    true,
    shift.value
  );
});
</script>

<style scoped>
.row {
  max-width: 1200px;
}

figure {
  border: 2px solid black;
}
</style>
