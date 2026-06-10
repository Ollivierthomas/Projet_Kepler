import data from './data_terre.json' with { type: 'json' };

const Sun = { x: 0, y: 0, z: 0, name: "Soleil" }; // Position du Soleil au centre du graphique

const graph_trajectoire = Plot.plot({
 x: {
    label: "points x →",
    grid: true                  // Affiche les lignes verticales de repère
  },
  
  // Configuration de l'axe Y (vertical)
  y: {
    label: "↑ points y", 
    grid: true,
  },

  marks: [
    Plot.ruleY([0], { stroke: "#000", strokeWidth: 0.25 }),// Affiche une ligne horizontale au niveau de y=0
    Plot.ruleX([0], { stroke: "#000", strokeWidth: 0.25 }),// Affiche une ligne verticale au niveau de x=0

    Plot.dot([Sun], { x: "x", y: "y", r: 10, fill: "yellow" }),// Affiche le Soleil au centre du graphique
  
    Plot.line(data, { x: "x", y: "y" , z: "z" }),// Affiche la trajectoire de Mercure en 2D (x et y)
    
  ]
})





const graph_energie = Plot.plot({
  x: {
      label: "temps →",
      grid: true // Affiche les lignes verticales de repère
    },
  y: {
    label: "↑ énergie (J)", 
    grid: true,
  },
  marks: [
    Plot.line(data, { x: "t", y: "ke", stroke: "firebrick" }),// Affiche l'énergie cinetique de Mercure en fonction du temps
    Plot.line(data, { x: "t", y: "pe", stroke: "steelblue" }),// Affiche l'énergie potentielle de Mercure en fonction du temps
    Plot.line(data, { x: "t", y: (d) => d.ke + d.pe, stroke: "green" }),// Affiche l'énergie totale en additionnant les énergies cinétique et potentielle de Mercure en fonction du temps
  ]
})

// On sélectionne notre div et on y injecte le graphique
document.getElementById("graph-container").append(graph_trajectoire);

document.getElementById("graph-container-2").append(graph_energie);