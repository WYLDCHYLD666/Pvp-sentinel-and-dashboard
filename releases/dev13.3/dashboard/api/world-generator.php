<?php
declare(strict_types=1);
header('Content-Type: application/json; charset=utf-8');
header('Cache-Control: no-store');

try {
    if ($_SERVER['REQUEST_METHOD'] !== 'POST') fail('POST required', 405);
    $action = (string)($_POST['action'] ?? 'validate');
    $id = normaliseId((string)($_POST['id'] ?? ''));
    if ($id === '') fail('Invalid world id', 400);
    $name = trim((string)($_POST['name'] ?? $id));
    $game = strtolower(trim((string)($_POST['game'] ?? 'dayz')));
    $size = (int)($_POST['world_size'] ?? 0);
    $origin = in_array($_POST['origin'] ?? '', ['south-west','north-west'], true) ? (string)$_POST['origin'] : 'south-west';
    if ($size < 1) fail('World size must be greater than zero', 400);

    $dashboard = realpath(__DIR__ . '/..');
    if ($dashboard === false) fail('Dashboard root unavailable', 500);
    $source = $dashboard . '/map-tools/import/' . $id;
    $destination = $dashboard . '/maps/' . $id;
    $validation = validateSource($source, $id);

    if ($action === 'validate') respond(['ok'=>true,'data'=>$validation]);
    if ($action !== 'generate') fail('Unsupported action', 400);
    if (!$validation['ready']) fail('Required source assets are missing. Run validation and add a topographic or satellite raster.', 422);

    if (!is_dir($destination) && !mkdir($destination, 0775, true) && !is_dir($destination)) fail('Unable to create world package directory', 500);
    $layersDir = $destination . '/layers';
    if (!is_dir($layersDir)) mkdir($layersDir, 0775, true);

    $copied=[];
    foreach (['top.jpg','satellite.jpg'] as $file) {
        if (is_file($source.'/'.$file)) { copy($source.'/'.$file, $destination.'/'.$file); $copied[]=$file; }
    }
    $vectorCounts=[];
    foreach (['roads','buildings','water','landmarks'] as $layer) {
        $src=$source.'/'.$layer.'.json';
        $dest=$layersDir.'/'.$layer.'.json';
        if (is_file($src)) {
            $data=readJsonAny($src);
            if ($data === null) fail('Invalid JSON in '.$layer.'.json', 422);
            $normalised=normaliseGeoJson($data, $layer);
            writeJson($dest,$normalised);
            $vectorCounts[$layer]=count($normalised['features']);
            $copied[]='layers/'.$layer.'.json';
        } else {
            writeJson($dest,['type'=>'FeatureCollection','features'=>[],'sentinel_layer'=>$layer]);
            $vectorCounts[$layer]=0;
        }
    }

    $sourceMeta=is_file($source.'/source.json') ? (readJsonAny($source.'/source.json') ?? []) : [];
    $manifest=[
      'schema'=>2,
      'package_format'=>'sentinel-world-package',
      'id'=>$id,
      'name'=>$name ?: $id,
      'game'=>$game,
      'world_size'=>$size,
      'bounds'=>['min_x'=>0,'min_z'=>0,'max_x'=>$size,'max_z'=>$size],
      'coordinates'=>['origin'=>$origin,'axis_x'=>'east','axis_z'=>'north','units'=>'metres'],
      'default_layer'=>is_file($destination.'/top.jpg')?'top':'sat',
      'generated_at'=>gmdate('c'),
      'generator'=>['name'=>'Sentinel World Package Builder','version'=>'13.3'],
      'source'=>array_merge(['type'=>'user-supplied-legal-source'],is_array($sourceMeta)?$sourceMeta:[]),
      'layers'=>[
        'raster'=>['topographic'=>is_file($destination.'/top.jpg')?'top.jpg':null,'satellite'=>is_file($destination.'/satellite.jpg')?'satellite.jpg':null],
        'vector'=>['roads'=>'layers/roads.json','buildings'=>'layers/buildings.json','water'=>'layers/water.json','landmarks'=>'layers/landmarks.json']
      ],
      'statistics'=>['vectors'=>$vectorCounts]
    ];
    writeJson($destination.'/world.json',$manifest);
    writeJson($destination.'/tile-manifest.json',[
      'schema'=>1,'mode'=>'single-raster-foundation','tile_size'=>256,'zoom_levels'=>[0],
      'note'=>'Dev13.1 records raster sources. Multi-resolution tile generation is connected in the next pipeline milestone.'
    ]);

    $manifestPath=$destination.'/world.json';
    $checksum=hash_file('sha256',$manifestPath) ?: '';
    $signingKey='/home/sentinel/config/world-signing.key';
    $signature=['status'=>'UNSIGNED','algorithm'=>'sha256','checksum'=>$checksum,'signed_at'=>null];
    if(is_readable($signingKey)){
      $key=trim((string)file_get_contents($signingKey));
      if($key!==''){
        $signature=['status'=>'SIGNED','algorithm'=>'hmac-sha256','checksum'=>$checksum,'value'=>hash_hmac('sha256',$checksum,$key),'signed_at'=>gmdate('c')];
      }
    }
    writeJson($destination.'/signature.json',$signature);
    ensureRegistry($dashboard.'/maps/registry.json',$id,$name,$game,$size,$origin);
    $checks=validateSource($source,$id)['checks'];
    respond(['ok'=>true,'data'=>['ready'=>true,'checks'=>$checks,'log'=>[
      'Source validation passed.',
      'Created maps/'.$id.'/world.json.',
      'Created tile-manifest.json foundation.',
      'Normalised '.array_sum($vectorCounts).' vector features.',
      'Copied '.count($copied).' source assets.',
      'Generated SHA-256 package checksum.',
      $signature['status']==='SIGNED'?'Package manifest signed.':'Package generated unsigned (no protected signing key configured).',
      'World package generated successfully.'
    ],'path'=>'maps/'.$id.'/','package_name'=>$id.'.swp','checksum'=>$checksum,'signature'=>$signature]]);
} catch (Throwable $e) { fail($e->getMessage(), 500); }

function validateSource(string $source,string $id): array {
  $exists=is_dir($source);
  $top=is_file($source.'/top.jpg'); $sat=is_file($source.'/satellite.jpg');
  $checks=[
    ['label'=>'Source staging folder','ok'=>$exists,'detail'=>$exists?'map-tools/import/'.$id:'Create map-tools/import/'.$id],
    ['label'=>'Raster source','ok'=>$top||$sat,'detail'=>$top&&$sat?'Topographic and satellite found':($top?'Topographic found':($sat?'Satellite found':'Add top.jpg or satellite.jpg'))],
  ];
  foreach(['roads','buildings','water','landmarks'] as $layer){
    $file=$source.'/'.$layer.'.json'; $ok=!is_file($file)||readJsonAny($file)!==null;
    $checks[]=['label'=>ucfirst($layer).' vector layer','ok'=>$ok,'detail'=>is_file($file)?($ok?'Valid JSON':'Invalid JSON'):'Optional — empty layer will be created'];
  }
  $ready=$exists&&($top||$sat)&&!array_filter($checks,fn($x)=>!$x['ok']&&str_contains($x['detail'],'Invalid'));
  return ['ready'=>$ready,'checks'=>$checks,'log'=>[$exists?'Source folder found.':'Source folder missing.',($top||$sat)?'Raster source detected.':'No raster source detected.',$ready?'Package is ready to generate.':'Package is not ready.']];
}
function normaliseGeoJson(array $data,string $layer): array {
  if (($data['type']??'')==='FeatureCollection' && is_array($data['features']??null)) { $data['sentinel_layer']=$layer; return $data; }
  if (array_is_list($data)) {
    $features=[]; foreach($data as $i=>$item){ if(!is_array($item))continue; $features[]=['type'=>'Feature','id'=>$item['id']??$layer.'-'.$i,'properties'=>$item['properties']??array_diff_key($item,['geometry'=>1,'coordinates'=>1]),'geometry'=>$item['geometry']??null]; }
    return ['type'=>'FeatureCollection','features'=>$features,'sentinel_layer'=>$layer];
  }
  return ['type'=>'FeatureCollection','features'=>[],'sentinel_layer'=>$layer,'source_payload'=>$data];
}
function ensureRegistry(string $file,string $id,string $name,string $game,int $size,string $origin): void {
  $r=is_file($file)?(readJsonAny($file)??['maps'=>[]]):['maps'=>[]]; if(!isset($r['maps'])||!is_array($r['maps']))$r['maps']=[];
  $found=false; foreach($r['maps'] as &$m){if(($m['id']??'')===$id){$m=array_merge($m,['id'=>$id,'name'=>$name,'game'=>$game,'world_size'=>$size,'coordinate_origin'=>$origin]);$found=true;break;}} unset($m);
  if(!$found)$r['maps'][]=['id'=>$id,'name'=>$name,'game'=>$game,'world_size'=>$size,'coordinate_origin'=>$origin,'source'=>'Sentinel Generator'];
  writeJson($file,$r);
}
function normaliseId(string $id): string { return preg_match('/^[a-z0-9_-]+$/',$id)?$id:''; }
function readJsonAny(string $file): ?array { $d=json_decode((string)@file_get_contents($file),true); return is_array($d)?$d:null; }
function writeJson(string $file,array $data): void { if(!is_dir(dirname($file)))mkdir(dirname($file),0775,true); if(file_put_contents($file,json_encode($data,JSON_PRETTY_PRINT|JSON_UNESCAPED_SLASHES).PHP_EOL,LOCK_EX)===false)throw new RuntimeException('Unable to write '.basename($file)); }
function respond(array $p): never { echo json_encode($p,JSON_UNESCAPED_SLASHES); exit; }
function fail(string $m,int $s): never { http_response_code($s); respond(['ok'=>false,'error'=>['message'=>$m]]); }
