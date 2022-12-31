use bevy::asset::AssetServerSettings;
use bevy::render::mesh::{Indices, PrimitiveTopology, VertexAttributeValues};
use bevy::sprite::Mesh2dHandle;
use bevy::{prelude::*, sprite::MaterialMesh2dBundle};

#[derive(Component)]
struct LightMesh;

struct Light {
    epoch: Vec2,
    head: Vec2,
    direction: Vec2,
}

impl Light {
    const LIGHT_COLOR: Color = Color::RED;
    const CLICK_RADIUS: f32 = 42.;

    fn new(epoch: Vec2) -> Self {
        Light {
            epoch,
            head: epoch,
            direction: Vec2::X,
        }
    }

    fn set_direction(&mut self, direction: Vec2) {
        self.direction = direction;
    }

    fn move_head(&mut self, factor: f32) {
        self.head += self.direction.normalize() * factor;
    }

    fn reset(&mut self) {
        self.head = self.epoch;
        self.direction = Vec2::X;
    }
}

impl From<&Light> for Mesh {
    fn from(light: &Light) -> Mesh {
        let vertices = [([light.epoch.x, light.epoch.y, 0.], [0., 0., 1.], [0., 0.])];
        let indices = Indices::U32(vec![0]);
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
}

#[derive(Copy, Clone)]
enum Mirror {
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

struct Mirrors {
    mirrors: Vec<Mirror>,
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
                    radius: 30.,
                },
            ],
        }
    }
}

impl Mirrors {
    const MIRROR_APPROACH_MARGIN: f32 = 0.5;
    const MIRROR_COLOR: Color = Color::rgb(0.62, 0.62, 0.62);

    fn try_reflect(&self, point: Vec2, scale: &mut f32) -> Option<Mirror> {
        for mirror in self.mirrors.iter() {
            match mirror {
                Mirror::Line { e1, e2 } => {
                    let mirror_len = (*e1 - *e2).length();
                    let dist_to_e1 = (point - *e1).length();
                    let dist_to_e2 = (point - *e2).length();
                    let closeness = (dist_to_e1 + dist_to_e2 - mirror_len).abs();
                    *scale = scale.min(closeness);
                    *scale = scale.max(1.);
                    if closeness <= Mirrors::MIRROR_APPROACH_MARGIN {
                        return Some(*mirror);
                    }
                }
                Mirror::Circle { center, radius } => {
                    let closeness = ((*center - point).length() - radius).abs();
                    *scale = scale.min(closeness);
                    *scale = scale.max(1.);
                    if closeness <= Mirrors::MIRROR_APPROACH_MARGIN {
                        return Some(*mirror);
                    }
                }
            }
        }
        None
    }
}

struct Target {
    center: Vec2,
    radius: f32,
}

fn circle_mesh(center: Vec2, radius: f32) -> Mesh {
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

fn add_point_to_mesh(mesh: &mut Mesh, point: Vec2) {
    let count = mesh.count_vertices();
    if let Some(VertexAttributeValues::Float32x3(positions)) =
        mesh.attribute_mut(Mesh::ATTRIBUTE_POSITION)
    {
        positions.push([point.x, point.y, 0.]);
    }
    if let Some(VertexAttributeValues::Float32x3(normals)) =
        mesh.attribute_mut(Mesh::ATTRIBUTE_NORMAL)
    {
        normals.push([0., 0., 1.]);
    }
    if let Some(VertexAttributeValues::Float32x2(uvs)) = mesh.attribute_mut(Mesh::ATTRIBUTE_UV_0) {
        uvs.push([0., 0.]);
    }
    if let Some(Indices::U32(indices)) = mesh.indices_mut() {
        indices.push(count as u32);
    }
}

const ROOM_WIDTH: f32 = 700.;
const ROOM_HEIGHT: f32 = 500.;
const ROOM_PADDING: f32 = 10.;

fn main() {
    let mut app = App::new();
    #[cfg(not(target_arch = "wasm32"))]
    {
        app.insert_resource(AssetServerSettings {
            asset_folder: "static/assets".to_string(),
            ..default()
        });
    }
    app.insert_resource(WindowDescriptor {
        width: ROOM_WIDTH + 2. * ROOM_PADDING,
        height: ROOM_HEIGHT + 2. * ROOM_PADDING,
        canvas: Some("#interactive_example".to_string()),
        fit_canvas_to_parent: true,
        ..default()
    })
    .insert_resource(ClearColor(Color::BLACK))
    .insert_resource(Light::new(Vec2::new(-180.0, 80.)))
    .insert_resource(Target {
        center: Vec2::new(150., -180.),
        radius: 24.,
    })
    .insert_resource(Mirrors::default())
    .add_plugins(DefaultPlugins)
    .add_startup_system(init)
    .add_system(game)
    .run();
}

fn init(
    mut commands: Commands,
    mut meshes: ResMut<Assets<Mesh>>,
    mut materials: ResMut<Assets<ColorMaterial>>,
    light: Res<Light>,
    mirrors: Res<Mirrors>,
    target: Res<Target>,
) {
    // Camera
    commands.spawn_bundle(Camera2dBundle::default());
    // Room
    commands.spawn_bundle(MaterialMesh2dBundle {
        mesh: meshes
            .add(Mesh::from(shape::Quad::new(Vec2::new(
                ROOM_WIDTH + 2. * ROOM_PADDING,
                ROOM_HEIGHT + 2. * ROOM_PADDING,
            ))))
            .into(),
        material: materials.add(Color::WHITE.into()),
        ..default()
    });
    commands.spawn_bundle(MaterialMesh2dBundle {
        mesh: meshes
            .add(Mesh::from(shape::Quad::new(Vec2::new(
                ROOM_WIDTH,
                ROOM_HEIGHT,
            ))))
            .into(),
        material: materials.add(Color::BLACK.into()),
        ..default()
    });
    // Mirrors
    for mirror in mirrors.mirrors.iter() {
        commands.spawn_bundle(MaterialMesh2dBundle {
            mesh: meshes.add(Mesh::from(mirror)).into(),
            material: materials.add(Mirrors::MIRROR_COLOR.into()),
            ..default()
        });
    }
    // Light
    commands
        .spawn_bundle(MaterialMesh2dBundle {
            mesh: meshes.add(Mesh::from(&*light)).into(),
            material: materials.add(Light::LIGHT_COLOR.into()),
            ..default()
        })
        .insert(LightMesh);
    // Click circle
    commands.spawn_bundle(MaterialMesh2dBundle {
        mesh: meshes
            .add(Mesh::from(circle_mesh(Vec2::ZERO, Light::CLICK_RADIUS)))
            .into(),
        material: materials.add(Color::YELLOW.into()),
        transform: Transform::from_xyz(light.epoch.x, light.epoch.y, 0.),
        ..default()
    });
    // Target
    commands.spawn_bundle(MaterialMesh2dBundle {
        mesh: meshes
            .add(Mesh::from(circle_mesh(Vec2::ZERO, target.radius)))
            .into(),
        material: materials.add(Color::GREEN.into()),
        transform: Transform::from_xyz(target.center.x, target.center.y, 0.),
        ..default()
    });
}

fn game(
    light_mesh_handle: Query<(&LightMesh, &Mesh2dHandle)>,
    mut meshes: ResMut<Assets<Mesh>>,
    mouse_button_input: Res<Input<MouseButton>>,
    mut windows: ResMut<Windows>,
    mut light: ResMut<Light>,
    target: Res<Target>,
    mirrors: Res<Mirrors>,
) {
    if mouse_button_input.just_pressed(MouseButton::Left) {
        let window = windows.primary_mut();
        if let Some(cursor) = window.physical_cursor_position() {
            let w = window.physical_width();
            let h = window.physical_height();
            let (x_hat, y_hat) = (
                cursor.x as f32 - w as f32 / 2.,
                cursor.y as f32 - h as f32 / 2.,
            );
            let scale_factor = window.scale_factor() as f32;
            let click = Vec2::new(x_hat / scale_factor, y_hat / scale_factor);
            info!("click = {:?}", click);
            let aim = click - light.epoch;
            if aim.length() > 0. && aim.length() > Light::CLICK_RADIUS {
                return;
            }
            light.reset();
            let (_, light_mesh_handle) = light_mesh_handle.single();
            let light_mesh = meshes.get_mut(&light_mesh_handle.0).unwrap();
            *light_mesh = Mesh::from(&*light);
            light.set_direction(aim);
            let mut scale = 1.;
            loop {
                light.move_head(scale);
                let hitting_target = (light.head - target.center).length() <= target.radius;
                if hitting_target {
                    info!("* target hit");
                    add_point_to_mesh(light_mesh, light.head);
                    return;
                }
                let hitting_wall =
                    light.head.x.abs() > ROOM_WIDTH / 2. || light.head.y.abs() > ROOM_HEIGHT / 2.;
                if hitting_wall {
                    info!("* wall hit");
                    add_point_to_mesh(light_mesh, light.head);
                    return;
                }
                if let Some(mirror) = mirrors.try_reflect(light.head, &mut scale) {
                    match mirror {
                        Mirror::Line { e1, e2 } => {
                            info!("* line mirror hit");
                            add_point_to_mesh(light_mesh, light.head);
                            let ray = light.direction.normalize();
                            let mirror = if ray.dot(e2 - e1) > 0. {
                                (e2 - e1).normalize()
                            } else {
                                (e1 - e2).normalize()
                            };
                            let mirror_perp = if ray.dot(Vec2::new(-mirror.y, mirror.x)) < 0. {
                                Vec2::new(-mirror.y, mirror.x).normalize()
                            } else {
                                Vec2::new(mirror.y, -mirror.x).normalize()
                            };
                            let along_the_mirror = ray.dot(mirror);
                            let perp_to_mirror = (1. - along_the_mirror * along_the_mirror).sqrt();
                            light.set_direction(
                                mirror * along_the_mirror + mirror_perp * perp_to_mirror,
                            );
                            light.move_head(2.);
                        }
                        Mirror::Circle { center, .. } => {
                            info!("* circle mirror hit");
                            add_point_to_mesh(light_mesh, light.head);
                            let ray = light.direction.normalize();
                            let mirror_perp_cap = (light.head - center).normalize();
                            let mirror_cap =
                                if ray.dot(Vec2::new(-mirror_perp_cap.y, mirror_perp_cap.x)) > 0. {
                                    Vec2::new(-mirror_perp_cap.y, mirror_perp_cap.x).normalize()
                                } else {
                                    Vec2::new(mirror_perp_cap.y, -mirror_perp_cap.x).normalize()
                                };
                            let perp_to_mirror = -1. * ray.dot(mirror_perp_cap);
                            let along_the_mirror = (1. - perp_to_mirror * perp_to_mirror).sqrt();
                            light.set_direction(
                                mirror_cap * along_the_mirror + mirror_perp_cap * perp_to_mirror,
                            );
                            light.move_head(2.);
                        }
                    }
                }
            }
        }
    }
}
