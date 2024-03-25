<template>
  <v-container align="center" fluid>
    <v-row class="row1">
      <v-col>
        <v-radio-group v-model="selectedRadio" color="blue">
          <v-radio label="SI Units" value="SI"></v-radio>
          <v-radio label="Imperial" value="imp"></v-radio>
        </v-radio-group>
      </v-col>
      <v-col cols="4">
        <v-sheet class="sheet">
          <p>Enter date range for Analysis</p>
          <v-divider></v-divider>
          <br />
          <v-text-field
            v-model="start"
            label="Start date"
            type="Date"
            dense
            solo-inverted
            class="mr-5"
            :style="{ maxWidth: '300px' }"
            flat
          ></v-text-field>
          <v-text-field
            v-model="end"
            label="End date"
            type="Date"
            dense
            solo-inverted
            class="mr-5"
            :style="{ maxWidth: '300px' }"
            flat
          ></v-text-field>
          <br />
          <v-btn
            text="Analyze"
            @click="
              updateLineChart();
              updateScatter();
            "
            color="primary"
            variant="tonal"
          ></v-btn>
        </v-sheet>
      </v-col>
      <v-col>
        <v-select
          v-model="selectedConversion"
          :items="conversionTypes"
          label="Select conversion type"
          outlined
        ></v-select
      ></v-col>
      <v-col>
        <v-text-field
          v-model="inputValue"
          label="Enter value"
          type="number"
          outlined
        ></v-text-field>
        <v-btn @click="convertValue" color="primary" variant="tonal"
          >Convert</v-btn
        >
        <br />
        <br />
        <br />
        <v-row
          ><v-alert v-if="result !== null" type="info" dense
            >Result: {{ result }}</v-alert
          ></v-row
        >
      </v-col>
    </v-row>
    <v-row class="row">
      <v-col cols="12">
        <figure class="highcharts-figure">
          <div id="container"></div>
        </figure>
      </v-col>

      <v-col cols="12">
        <figure class="highcharts-figure">
          <div id="container3"></div>
        </figure>
      </v-col>
    </v-row>
  </v-container>
</template>

<script setup>
/** JAVASCRIPT HERE */

import Highcharts from "highcharts";
import more from "highcharts/highcharts-more";
import Exporting from "highcharts/modules/exporting";
import { withDirectives } from "vue";
Exporting(Highcharts);
more(Highcharts);

// IMPORTS
import { useMqttStore } from "@/store/mqttStore"; // Import Mqtt Store
import { storeToRefs } from "pinia";

import { useAppStore } from "@/store/appStore";
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
const Mqtt = useMqttStore();
const AppStore = useAppStore();
const router = useRouter();
const route = useRoute();
var start = ref(null);
var end = ref(null);
const relationLine = ref(null); // Chart object
var selectedRadio = ref(null);
const scatter = ref(null); // Chart object
const selectedConversion = ref(null);
var inputValue = ref(null);
var result = ref(null);
var conversionTypes = [
  { title: "Meters to Yards", value: "m_to_yd" },
  { title: "Yards to Meters", value: "yd_to_m" },
  { title: "Kilopascals to Inch of Merury", value: "kpa_to_hg" },
  { title: "Inch of Merury to Kilopascals", value: "hg_to_kpa" },
  { title: "Celsius to Fahrenheit", value: "c_to_f" },
  { title: "Fahrenheit to Celsius", value: "f_to_c" },
];

// FUNCTIONS

const CreateCharts = async () => {
  // TEMPERATURE CHART
  relationLine.value = Highcharts.chart("container", {
    chart: {
      events: {
        load() {
          console.log(this.options.colors);
        },
      },
    },
    chart: { zoomType: "x" },
    title: {
      text: "Temperature, Heat Index and Humidity Analysis",
      align: "left",
    },
    subtitle: {
      text: "Chart showing the relationship between Temperature, Heat Index and Humidity. How does Temperature and Humidity influences Heat Index?",
    },
    yAxis: {
      title: {
        text: "Air Temperature & Heat Index & Humidity",
        style: { color: "#000000" },
      },
    },

    tooltip: {
      shared: true,
    },

    xAxis: {
      type: "datetime",
      title: { text: "Time", style: { color: "#000000" } },
    },
    colors: ["red", "blue", "green"],
    series: [
      {
        name: "Temperature",
        type: "line",
        data: [],
        turboThreshold: 0,
      },
      {
        name: "Heat Index",
        type: "line",
        data: [],
        turboThreshold: 0,
      },
      {
        name: "Humidity",
        type: "line",
        data: [],
        turboThreshold: 0,
      },
    ],
  });

  scatter.value = Highcharts.chart("container3", {
    chart: { zoomType: "x" },
    title: {
      text: "Altitude & Pressure Correlation Analysis",
      align: "left",
    },
    subtitle: {
      text: "Visualize the relationship between Altitude and Pressure as well as revealing patterns or trends in the data",
    },
    yAxis: {
      title: {
        text: "Altitude",
        style: { color: "#000000" },
      },
      labels: { format: "{value}" },
    },

    xAxis: {
      title: { text: "Pressure", style: { color: "#000000" } },
      labels: { format: "{value}" },
    },
    tooltip: {
      shared: true,
    },
    series: [
      {
        name: "Analysis",
        type: "scatter",
        data: [],
        turboThreshold: 0,
        color: Highcharts.getOptions().colors[5],
      },
    ],
  });
};

onMounted(() => {
  // THIS FUNCTION IS CALLED AFTER THIS COMPONENT HAS BEEN MOUNTED
  Mqtt.connect(); // Connect to Broker located on the backend
  setTimeout(() => {
    // Subscribe to each topic
    Mqtt.subscribe("620153775");
    Mqtt.subscribe("620153775_sub");
  }, 3000);
  CreateCharts();
});

onBeforeUnmount(() => {
  // THIS FUNCTION IS CALLED RIGHT BEFORE THIS COMPONENT IS UNMOUNTED
  Mqtt.unsubcribeAll();
});

const convertValue = async () => {
  const value = parseFloat(inputValue.value);
  if (isNaN(value)) {
    result.value = "Please enter a valid number";
    return;
  }
  switch (selectedConversion.value) {
    case "m_to_yd":
      result.value = `${value} meters is ${(value * 1.094).toFixed(3)} yards`;
      break;
    case "yd_to_m":
      result.value = `${value} yards is ${(value / 1.094).toFixed(3)} meters`;
      break;
    case "kpa_to_hg":
      result.value = `${value} kilopascals is ${(value / 3.386).toFixed(
        2
      )} Inch of Merury`;
      break;
    case "hg_to_kpa":
      result.value = `${value} Inch of Merury is ${(value * 3.386).toFixed(
        2
      )} kilopascals`;
      break;
    case "c_to_f":
      result.value = `${value}° Celsius is ${((value * 9) / 5 + 32).toFixed(
        2
      )}° Fahrenheit`;
      break;
    case "f_to_c":
      result.value = `${value}° Fahrenheit is ${(
        ((value - 32) * 5) /
        9
      ).toFixed(2)}° Celsius`;
      break;
    default:
      result.value = "Please select a conversion type";
  }
};

const updateLineChart = async () => {
  if (!!start.value && !!end.value) {
    // Convert output from Textfield components to 10 digit timestamps
    let startDate = new Date(start.value).getTime() / 1000;
    let endDate = new Date(end.value).getTime() / 1000;
    // Fetch data from backend
    const data = await AppStore.getAllInRange(startDate, endDate);
    // Create arrays for each plot
    let temperature = [];
    let heatindex = [];
    let humidity = [];
    // Iterate through data variable and transform object to format recognized by highcharts
    if (selectedRadio.value == "imp") {
      data.forEach((row) => {
        temperature.push({
          x: row.timestamp * 1000,
          y: parseFloat((row.temperature * 1.8 + 32).toFixed(2)),
        });

        heatindex.push({
          x: row.timestamp * 1000,
          y: parseFloat((row.heatindex * 1.8 + 32).toFixed(2)),
        });
        humidity.push({
          x: row.timestamp * 1000,
          y: parseFloat(row.humidity.toFixed(2)),
        });
      });
    } else {
      data.forEach((row) => {
        temperature.push({
          x: row.timestamp * 1000,
          y: parseFloat(row.temperature.toFixed(2)),
        });

        heatindex.push({
          x: row.timestamp * 1000,
          y: parseFloat(row.heatindex.toFixed(2)),
        });
        humidity.push({
          x: row.timestamp * 1000,
          y: parseFloat(row.humidity.toFixed(2)),
        });
      });
    }
    // Add data to Temperature and Heat Index chart
    relationLine.value.series[0].setData(temperature);
    relationLine.value.series[1].setData(heatindex);
    relationLine.value.series[2].setData(humidity);
  }
};

const updateScatter = async () => {
  if (!!start.value && !!end.value) {
    // Convert output from Textfield components to 10 digit timestamps
    let startDate = new Date(start.value).getTime() / 1000;
    let endDate = new Date(end.value).getTime() / 1000;
    // Fetch data from backend
    const data = await AppStore.getAllInRange(startDate, endDate);
    // Create arrays for each plot
    let scatterPoints = [];
    // Iterate through data variable and transform object to format recognized by highcharts
    if (selectedRadio.value == "imp") {
      data.forEach((row) => {
        scatterPoints.push({
          y: parseFloat((row.altitude * 1.094).toFixed(2)),
          x: parseFloat((row.pressure / 3.386).toFixed(2)),
        });
      });
    } else {
      data.forEach((row) => {
        scatterPoints.push({
          y: parseFloat(row.altitude.toFixed(2)),
          x: parseFloat(row.pressure.toFixed(2)),
        });
      });
    }

    // Add data to Temperature and Heat Index chart
    scatter.value.series[0].setData(scatterPoints);
  }
};
</script>

<style scoped>
/** CSS STYLE HERE */

.container {
  background-color: #f5f5f5;
  width: 1200px;
}

.row {
  max-width: 1200px;
}

.row1 {
  max-width: 1200px;
  padding: 1;
}

.col1 {
  border: black;
}

.sheet {
  padding: 2;
  height: 250;
}

Figure {
  border: 2px solid black;
}
</style>
