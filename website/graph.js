import data from './data_terre.json' with { type: 'json' };

// On trouve les valeurs maximales absolues pour les coordonnées x et y afin de définir les limites du graphique
const maxX = Math.max(...data.map(d => Math.abs(d.x)));
const maxY = Math.max(...data.map(d => Math.abs(d.y)));

const max = Math.max(maxX, maxY);

// graphique de la trajectoire de la Terre autour du Soleil
const Sun = { x: 0, y: 0, z: 0, name: "Soleil" }; // Position du Soleil au centre du graphique

const graph_trajectoire = Plot.plot({

   // Taille du graphe
  width: 900,
  height: 900,
  aspectRatio: 1,
  grid:true,
  // Marges
  marginLeft: 80,
  marginRight: 80,
  marginTop: 80,
  marginBottom: 80,

  title: "Trajectoire de la Terre autour du Soleil",
  

 x: {
    label: "points x →",
    // Affiche les lignes verticales de repère
    tickFormat: d => `${d / 1e9} Gm`,// Valeurs plus lisibles
    },
  
  // Configuration de l'axe Y (vertical)
  y: {
    label: "↑ points y", 
    tickFormat: d => `${d / 1e9} Gm`
  },
  
  marks: [
    Plot.ruleY([0], { stroke: "#fff15a9d", strokeWidth: 0.25 }),// Affiche une ligne horizontale au niveau de y=0
    Plot.ruleX([0], { stroke: "#fff15a9d", strokeWidth: 0.25 }),// Affiche une ligne verticale au niveau de x=0

    Plot.dot([Sun], { x: "x", y: "y", r: 10, fill: "yellow" ,stroke:"orange", strokeWidth: 2 , tip :true ,title:() => "Soleil" }),// Affiche le Soleil au centre du graphique
  
    Plot.line(data, { x: "x", y: "y"  }),// Affiche la trajectoire de la Terre en 2D (x et y)
    
    
  ]
})



const graph_energie = Plot.plot({

  width: 900,
  height: 500,

  marginLeft: 90,

  title: "Énergies du système",

  x: {label: "Temps →", grid: true},

  y: {label: "↑ Énergie (J)",grid: true, tickFormat: d => `${(d / 1e33).toFixed(1)}e33`},

  color: {
    legend: true
  },

  marks: [

    Plot.line(data, {x: "t",y: "ke",stroke: "red"}),// Affiche l'énergie cinétique en rouge

    Plot.line(data, {x: "t", y: "pe",stroke: "blue"}),// Affiche l'énergie potentielle en bleu

    Plot.line(data, {x: "t", y: (d) => d.ke + d.pe, stroke: "green"})// Affiche l'énergie totale (cinétique + potentielle) en vert
  ]
});

// On sélectionne notre div et on y injecte le graphique
document.getElementById("graph-container").append(graph_trajectoire);

document.getElementById("graph-container-2").append(graph_energie);