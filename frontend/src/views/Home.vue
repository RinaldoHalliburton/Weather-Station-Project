<template>
  <VContainer class="fill-height">
    <VResponsive class="align-center text-center fill-height">
      <VImg height="350" class="mb-5" src="/weather.png" />

      <div class="text-body-2 font-weight-light mb-n1">Remote</div>

      <h1 class="text-h2 font-weight-bold">Weather Monitoring</h1>
      <div class="text-body-1 font-weight-light mt-1">Station</div>

      <div class="py-14" />

      <VRow class="d-flex align-center justify-center">
        <VCol cols="auto">
          <VBtn
            color="primary"
            href="https://github.com/rinaldo-256int/Weather-Station-Project/tree/master"
            min-width="228"
            rel="noopener noreferrer"
            size="x-large"
            target="_blank"
            variant="flat"
          >
            <v-icon>mdi-github</v-icon>

            GitHub
          </VBtn>
        </VCol>
      </VRow>
    </VResponsive>
  </VContainer>
</template>

<script setup>
import { storeToRefs } from "pinia";
import { useMqttStore } from "@/store/mqttStore";
import { ref, reactive, watch, onMounted, onBeforeUnmount } from "vue";

// VARIABLES

// FUNCTIONS
onMounted(() => {});

onBeforeUnmount(() => {});
</script>
