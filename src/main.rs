use bevy::asset::AssetServerSettings;
use bevy::sprite::Mesh2dHandle;
use bevy::{prelude::*, sprite::MaterialMesh2dBundle};

mod light;
use light::*;
mod mirrors;
use mirrors::*;
mod target;
use target::*;

const ROOM_WIDTH: f32 = 700.;
const ROOM_HEIGHT: f32 = 500.;
const ROOM_PADDING: f32 = 10.;
const LIGHT_COLOR: Color = Color::RED;
const MIRROR_APPROACH_MARGIN: f32 = 0.5;
const MIRROR_COLOR: Color = Color::WHITE;

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
        canvas: Some("#interactive".to_string()),
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
            material: materials.add(MIRROR_COLOR.into()),
            ..default()
        });
    }
    // Light
    commands
        .spawn_bundle(MaterialMesh2dBundle {
            mesh: meshes.add(Mesh::from(&*light)).into(),
            material: materials.add(LIGHT_COLOR.into()),
            ..default()
        })
        .insert(LightMarker);
    // Click circle
    commands.spawn_bundle(MaterialMesh2dBundle {
        mesh: meshes.add(Mesh::from(circle_mesh(Vec2::ZERO, 4.))).into(),
        material: materials.add(Color::YELLOW.into()),
        transform: Transform::from_xyz(light.epoch.x, light.epoch.y, 0.),
        ..default()
    });
    // Target
    commands
        .spawn_bundle(MaterialMesh2dBundle {
            mesh: meshes
                .add(Mesh::from(circle_mesh(Vec2::ZERO, target.radius)))
                .into(),
            material: materials.add(Color::GREEN.into()),
            transform: Transform::from_xyz(target.center.x, target.center.y, 0.),
            ..default()
        })
        .insert(TargetMarker);
}

fn game(
    light_mesh_handle: Query<(&LightMarker, &Mesh2dHandle)>,
    target_material_handle: Query<(&TargetMarker, &Handle<ColorMaterial>)>,
    mut meshes: ResMut<Assets<Mesh>>,
    mut materials: ResMut<Assets<ColorMaterial>>,
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
            if aim.length() == 0. {
                return;
            }
            light.reset();
            let (_, light_mesh_handle) = light_mesh_handle.single();
            let (_, target_material_handle) = target_material_handle.single();
            let light_mesh = meshes.get_mut(&light_mesh_handle.0).unwrap();
            *light_mesh = Mesh::from(&*light);
            light.set_direction(aim);
            let target_material = materials.get_mut(&target_material_handle).unwrap();
            target_material.color = Color::GREEN;
            let mut scale = 1.;
            loop {
                light.move_head(scale);
                let hitting_target = (light.head - target.center).length() <= target.radius;
                if hitting_target {
                    info!("* target hit");
                    light.add_point_to_mesh(light_mesh);
                    let target_material = materials.get_mut(&target_material_handle).unwrap();
                    target_material.color = Color::RED;
                    return;
                }
                let hitting_wall =
                    light.head.x.abs() > ROOM_WIDTH / 1. || light.head.y.abs() > ROOM_HEIGHT / 2.;
                if hitting_wall {
                    info!("* wall hit");
                    light.add_point_to_mesh(light_mesh);
                    return;
                }
                if let Some(mirror) = mirrors.check_reflection(light.head, &mut scale) {
                    match mirror {
                        Mirror::Line { e1, e2 } => {
                            info!("* line mirror hit");
                            light.add_point_to_mesh(light_mesh);
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
                            light.add_point_to_mesh(light_mesh);
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
