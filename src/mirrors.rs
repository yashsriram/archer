use bevy::prelude::*;
use bevy::render::mesh::{Indices, PrimitiveTopology};

#[derive(Copy, Clone)]
pub enum Mirror {
    Circle { center: Vec2, radius: f32 },
    Line { e1: Vec2, e2: Vec2 },
}

impl From<&Mirror> for Mesh {
    fn from(mirror: &Mirror) -> Mesh {
        match mirror {
            Mirror::Line { e1, e2 } => {
                let vertices = [
                    ([e1.x, e1.y, 0.], [0., 0., 1.], [0., 0.]),
                    ([e2.x, e2.y, 0.], [0., 0., 1.], [0., 0.]),
                ];
                let indices = Indices::U32(vec![0, 1]);
                let positions: Vec<_> = vertices.iter().map(|(p, _, _)| *p).collect();
                let normals: Vec<_> = vertices.iter().map(|(_, p, _)| *p).collect();
                let uvs: Vec<_> = vertices.iter().map(|(_, _, uv)| *uv).collect();
                let mut mesh = Mesh::new(PrimitiveTopology::LineList);
                mesh.set_indices(Some(indices));
                mesh.insert_attribute(Mesh::ATTRIBUTE_POSITION, positions);
                mesh.insert_attribute(Mesh::ATTRIBUTE_NORMAL, normals);
                mesh.insert_attribute(Mesh::ATTRIBUTE_UV_0, uvs);
                mesh
            }
            Mirror::Circle { center, radius } => circle_mesh(*center, *radius),
        }
    }
}

pub struct Mirrors {
    pub mirrors: Vec<Mirror>,
}

impl Default for Mirrors {
    fn default() -> Self {
        Self {
            mirrors: vec![
                Mirror::Line {
                    e1: Vec2::new(-200., 150.),
                    e2: Vec2::new(180., 150.),
                },
                Mirror::Line {
                    e1: Vec2::new(-200., 10.),
                    e2: Vec2::new(100., 10.),
                },
                Mirror::Line {
                    e1: Vec2::new(180., 150.),
                    e2: Vec2::new(180., -150.),
                },
                Mirror::Line {
                    e1: Vec2::new(100., 10.),
                    e2: Vec2::new(100., -150.),
                },
                Mirror::Circle {
                    center: Vec2::new(0., 80.),
                    radius: 10.,
                },
                Mirror::Circle {
                    center: Vec2::new(50., 80.),
                    radius: 10.,
                },
            ],
        }
    }
}

impl Mirrors {
    pub fn try_reflect(&self, point: Vec2, scale: &mut f32) -> Option<Mirror> {
        for mirror in self.mirrors.iter() {
            match mirror {
                Mirror::Line { e1, e2 } => {
                    let mirror_len = (*e1 - *e2).length();
                    let dist_to_e1 = (point - *e1).length();
                    let dist_to_e2 = (point - *e2).length();
                    let closeness = (dist_to_e1 + dist_to_e2 - mirror_len).abs();
                    *scale = scale.min(closeness);
                    *scale = scale.max(1.);
                    if closeness <= super::MIRROR_APPROACH_MARGIN {
                        return Some(*mirror);
                    }
                }
                Mirror::Circle { center, radius } => {
                    let closeness = ((*center - point).length() - radius).abs();
                    *scale = scale.min(closeness);
                    *scale = scale.max(1.);
                    if closeness <= super::MIRROR_APPROACH_MARGIN {
                        return Some(*mirror);
                    }
                }
            }
        }
        None
    }
}

pub fn circle_mesh(center: Vec2, radius: f32) -> Mesh {
    let resolution = radius as usize;
    let vertices: Vec<_> = (1..=resolution)
        .map(|idx| {
            (
                [
                    center.x
                        + radius
                            * (2. * std::f32::consts::PI * idx as f32 / resolution as f32).cos(),
                    center.y
                        + radius
                            * (2. * std::f32::consts::PI * idx as f32 / resolution as f32).sin(),
                    0.,
                ],
                [0., 0., 1.],
                [0., 0.],
            )
        })
        .collect();
    let mut indices: Vec<_> = (0..resolution).map(|e| e as u32).collect();
    indices.push(0);
    let indices = Indices::U32(indices);
    let positions: Vec<_> = vertices.iter().map(|(p, _, _)| *p).collect();
    let normals: Vec<_> = vertices.iter().map(|(_, p, _)| *p).collect();
    let uvs: Vec<_> = vertices.iter().map(|(_, _, uv)| *uv).collect();
    let mut mesh = Mesh::new(PrimitiveTopology::LineStrip);
    mesh.set_indices(Some(indices));
    mesh.insert_attribute(Mesh::ATTRIBUTE_POSITION, positions);
    mesh.insert_attribute(Mesh::ATTRIBUTE_NORMAL, normals);
    mesh.insert_attribute(Mesh::ATTRIBUTE_UV_0, uvs);
    mesh
}
