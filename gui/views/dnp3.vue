<script setup>
import { inject, onMounted, ref } from "vue";

const $api = inject("$api");

const name = ref('');
const description = ref('');
const abilities = ref([]);

onMounted(async () => {
    const response = await $api.get('/plugin/dnp3/data')
    const pluginData = await response.data

    name.value = pluginData.name
    description.value = pluginData.description
    abilities.value = pluginData.abilities
});

function handleBoxExit(e){
    e.target.scrollTo({top: 0, behavior: 'smooth'});
}
</script>

<template>
    <div>
        <h2>{{ name }} Protocol Plugin</h2>
        <p>{{ description }}</p>
    </div>
    <hr>

    <h3>Documentation</h3>
    <p>
        Get started by referencing the
        <a :href="`/docs/plugins/${name.toLowerCase()}/${name.toLowerCase()}.html`" target="_blank">{{ name }} Plugin Fieldmanual</a>.
    </p>

    <h3>Abilities</h3>
    <table class="table abilities-table">
        <thead>
            <tr class="table-header">
                <th>Name</th>
                <th>ATT&amp;CK Technique</th>
                <th>Description</th>
            </tr>
        </thead>
        <tbody>
            <tr v-for="a in abilities">
                <td>{{a.name}}</td>
                <td>
                    <div class="abilitytags">
                        <span class="tag tactic is-dark m-0 mb-1">{{a.tactic}}</span><br>
                    </div>
                    {{a.technique_name}} ({{a.technique_id}})
                </td>
                <td v-html="a.description"></td>
            </tr>
        </tbody>
    </table>
    <h3>Adversaries</h3>
    <p>There are no custom {{ name }} adversaries at this time.</p>
</template>

<style scoped>
h2 {
    font-size: 1.75rem;
    line-height: 1.75rem;
    font-weight: 800;
    padding: 1rem 0;
}
h3 {
    font-size: 1.5rem;
    line-height: 1.75rem;
    font-weight: 800;
    padding: 1rem 0;
}
hr {
    margin: 1rem 0;
}
p {
    padding: 0 0 1rem 0;
}
a {
    color: #D6B8EF; 
}
a:hover {
    color: #8b00ff; 
}
.abilities-table td:nth-child(1) { /* Ability name column */
    font-weight: bold;
}
.abilities-table td:nth-child(3) { /* Description column */
    width: 40%;
}
.abilities-table tbody tr:nth-child(odd) {
    background-color: rgba(139, 0, 255, 0.1); 
}
.abilities-table tbody tr:hover {
    background-color: rgba(139, 0, 255, 0.3); 
}
</style>
